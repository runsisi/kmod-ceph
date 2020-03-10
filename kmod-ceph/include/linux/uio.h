/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *	Berkeley style UIO structures	-	Alan Cox 1994.
 */
#ifndef __KC_LINUX_UIO_H
#define __KC_LINUX_UIO_H

#include_next <linux/uio.h>
#include <linux/fs.h>

struct page;

enum iter_type {
	/* iter types */
	ITER_IOVEC = 4,
	ITER_KVEC = 8,
	ITER_BVEC = 16,
};

struct iov_bvec_iter {
	/*
	 * Bit 0 is the read/write bit, set if we're writing.
	 * Bit 1 is the BVEC_FLAG_NO_REF bit, set if type is a bvec and
	 * the caller isn't expecting to drop a page reference when done.
	 */
	unsigned int type;
	size_t iov_offset;
	size_t count;
	union {
		const struct bio_vec *bvec;
	};
	union {
		unsigned long nr_segs;
	};
};

void _iov_iter_advance(struct iov_iter *i, size_t bytes);
size_t iov_iter_zero(size_t bytes, struct iov_iter *);

size_t iov_bvec_iter_zero(size_t bytes, struct iov_bvec_iter *i);
void iov_bvec_iter_advance(struct iov_bvec_iter *i, size_t size);
void iov_iter_bvec(struct iov_bvec_iter *i, unsigned int direction, const struct bio_vec *bvec,
			unsigned long nr_segs, size_t count);

ssize_t iov_iter_get_pages(struct iov_iter *i, unsigned int type, struct page **pages,
			size_t maxsize, unsigned maxpages, size_t *start);
int iov_iter_npages(const struct iov_iter *i, int maxpages);

/*
 * reexpand a previously truncated iterator; count must be no more than how much
 * we had shrunk it.
 */
static inline void iov_iter_reexpand(struct iov_iter *i, size_t count)
{
	i->count = count;
}

#endif
