// SPDX-License-Identifier: GPL-2.0-only
#include <linux/export.h>
#include <linux/bvec.h>
#include <linux/uio.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/splice.h>
#include <net/checksum.h>
#include <linux/scatterlist.h>

#define iterate_iovec(i, n, __v, __p, skip, STEP) {	\
	size_t left;					\
	size_t wanted = n;				\
	__p = i->iov;					\
	__v.iov_len = min(n, __p->iov_len - skip);	\
	if (likely(__v.iov_len)) {			\
		__v.iov_base = __p->iov_base + skip;	\
		left = (STEP);				\
		__v.iov_len -= left;			\
		skip += __v.iov_len;			\
		n -= __v.iov_len;			\
	} else {					\
		left = 0;				\
	}						\
	while (unlikely(!left && n)) {			\
		__p++;					\
		__v.iov_len = min(n, __p->iov_len);	\
		if (unlikely(!__v.iov_len))		\
			continue;			\
		__v.iov_base = __p->iov_base;		\
		left = (STEP);				\
		__v.iov_len -= left;			\
		skip = __v.iov_len;			\
		n -= __v.iov_len;			\
	}						\
	n = wanted - n;					\
}

#define iterate_bvec(i, n, __v, __bi, skip, STEP) {	\
	struct bvec_iter __start;			\
	__start.bi_size = n;				\
	__start.bi_bvec_done = skip;			\
	__start.bi_idx = 0;				\
	for_each_bvec(__v, i->bvec, __bi, __start) {	\
		if (!__v.bv_len)			\
			continue;			\
		(void)(STEP);				\
	}						\
}

static void memzero_page(struct page *page, size_t offset, size_t len)
{
	char *addr = kmap_atomic(page);
	memset(addr + offset, 0, len);
	kunmap_atomic(addr);
}

size_t iov_iter_zero(size_t bytes, struct iov_iter *i)
{
	if (unlikely(i->count < bytes))
		bytes = i->count;
	if (i->count) {
		size_t skip = i->iov_offset;
		{
			const struct iovec *iov;
			struct iovec v;
			iterate_iovec(i, bytes, v, iov, skip, (clear_user(v.iov_base, v.iov_len)))
			if (skip == iov->iov_len) {
				iov++;
				skip = 0;
			}
			i->nr_segs -= iov - i->iov;
			i->iov = iov;
		}
		i->count -= bytes;
		i->iov_offset = skip;
	}
	return bytes;
}
//EXPORT_SYMBOL(iov_iter_zero);

void _iov_iter_advance(struct iov_iter *i, size_t size)
{
	if (unlikely(i->count < size))
		size = i->count;
	if (i->count) {
		size_t skip = i->iov_offset;
		{
			const struct iovec *iov;
			struct iovec v;
			iterate_iovec(i, size, v, iov, skip, (0))
			if (skip == iov->iov_len) {
				iov++;
				skip = 0;
			}
			i->nr_segs -= iov - i->iov;
			i->iov = iov;
		}
		i->count -= size;
		i->iov_offset = skip;
	}
}
//EXPORT_SYMBOL(iov_iter_advance);

size_t iov_bvec_iter_zero(size_t bytes, struct iov_bvec_iter *i)
{
	if (unlikely(i->count < bytes))
		bytes = i->count;
	if (i->count) {
		size_t skip = i->iov_offset;
		{
			const struct bio_vec *bvec = i->bvec;
			struct bio_vec v;
			struct bvec_iter __bi;
			iterate_bvec(i, bytes, v, __bi, skip, (memzero_page(v.bv_page, v.bv_offset, v.bv_len)))
			i->bvec = __bvec_iter_bvec(i->bvec, __bi);
			i->nr_segs -= i->bvec - bvec;
			skip = __bi.bi_bvec_done;
		}
		i->count -= bytes;
		i->iov_offset = skip;
	}
	return bytes;
}
//EXPORT_SYMBOL(iov_iter_zero);

void iov_bvec_iter_advance(struct iov_bvec_iter *i, size_t size)
{
	if (unlikely(i->count < size))
		size = i->count;
	if (i->count) {
		size_t skip = i->iov_offset;
		{
			const struct bio_vec *bvec = i->bvec;
			struct bio_vec v;
			struct bvec_iter __bi;
			iterate_bvec(i, size, v, __bi, skip, (0))
			i->bvec = __bvec_iter_bvec(i->bvec, __bi);
			i->nr_segs -= i->bvec - bvec;
			skip = __bi.bi_bvec_done;
		}
		i->count -= size;
		i->iov_offset = skip;
	}
}
//EXPORT_SYMBOL(iov_iter_advance);

void iov_iter_bvec(struct iov_bvec_iter *i, unsigned int direction,
			const struct bio_vec *bvec, unsigned long nr_segs,
			size_t count)
{
	WARN_ON(direction & ~(READ | WRITE));
	i->type = ITER_BVEC | (direction & (READ | WRITE));
	i->bvec = bvec;
	i->nr_segs = nr_segs;
	i->iov_offset = 0;
	i->count = count;
}
//EXPORT_SYMBOL(iov_iter_bvec);

ssize_t iov_iter_get_pages(struct iov_iter *i, unsigned int type,
		   struct page **pages, size_t maxsize, unsigned maxpages,
		   size_t *start)
{
	if (maxsize > i->count)
		maxsize = i->count;

	if (likely(maxsize)) {
		size_t skip = i->iov_offset;
		const struct iovec *iov;
		struct iovec v;
		iterate_iovec(i, maxsize, v, iov, skip, ({
			unsigned long addr = (unsigned long)v.iov_base;
			size_t len = v.iov_len + (*start = addr & (PAGE_SIZE - 1));
			int n;
			int res;

			if (len > maxpages * PAGE_SIZE)
				len = maxpages * PAGE_SIZE;
			addr &= ~(PAGE_SIZE - 1);
			n = DIV_ROUND_UP(len, PAGE_SIZE);
			res = get_user_pages_fast(addr, n,
					(type & (READ | WRITE)) != WRITE ?  FOLL_WRITE : 0,
					pages);
			if (unlikely(res < 0))
				return res;
			return (res == n ? len : res * PAGE_SIZE) - *start;
		0;})
		)
	}
	return 0;
}
//EXPORT_SYMBOL(iov_iter_get_pages);

int iov_iter_npages(const struct iov_iter *i, int maxpages)
{
	size_t size = i->count;
	int npages = 0;

	if (!size)
		return 0;

	{
		size_t skip = i->iov_offset;
		const struct iovec *iov;
		struct iovec v;
		iterate_iovec(i, size, v, iov, skip, ({
			unsigned long p = (unsigned long)v.iov_base;
			npages += DIV_ROUND_UP(p + v.iov_len, PAGE_SIZE)
				- p / PAGE_SIZE;
			if (npages >= maxpages)
				return maxpages;
		0;})
		)
	}
	return npages;
}
//EXPORT_SYMBOL(iov_iter_npages);
