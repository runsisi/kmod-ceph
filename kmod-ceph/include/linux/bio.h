/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2001 Jens Axboe <axboe@suse.de>
 */
#ifndef __KC_LINUX_BIO_H
#define __KC_LINUX_BIO_H

#include_next <linux/bio.h>
#include <linux/bvec.h>

#ifdef CONFIG_BLOCK

#define bio_iter_iovec(bio, iter)				\
	bvec_iter_bvec((bio)->bi_io_vec, (iter))

#define bio_iter_page(bio, iter)				\
	bvec_iter_page((bio)->bi_io_vec, (iter))
#define bio_iter_len(bio, iter)					\
	bvec_iter_len((bio)->bi_io_vec, (iter))
#define bio_iter_offset(bio, iter)				\
	bvec_iter_offset((bio)->bi_io_vec, (iter))

#define bvec_iter_sectors(iter)	((iter).bi_size >> 9)
#define bvec_iter_end_sector(iter) ((iter).bi_sector + bvec_iter_sectors((iter)))

static inline bool bio_no_advance_iter(struct bio *bio)
{
	return bio_op(bio) == REQ_OP_DISCARD ||
//	       bio_op(bio) == REQ_OP_SECURE_ERASE ||
	       bio_op(bio) == REQ_OP_WRITE_SAME;
//	       bio_op(bio) == REQ_OP_WRITE_ZEROES;
}

static inline void bio_advance_iter(struct bio *bio, struct bvec_iter *iter,
				    unsigned bytes)
{
	iter->bi_sector += bytes >> 9;

	if (bio_no_advance_iter(bio))
		iter->bi_size -= bytes;
	else
		bvec_iter_advance(bio->bi_io_vec, iter, bytes);
		/* TODO: It is reasonable to complete bio with error here. */
}

#define __kc_bio_for_each_segment(bvl, bio, iter, start)			\
	for (iter = (start);						\
	     (iter).bi_size &&						\
		((bvl = bio_iter_iovec((bio), (iter))), 1);		\
	     bio_advance_iter((bio), &(iter), (bvl).bv_len))

#define kc_bio_for_each_segment(bvl, bio, iter)				\
	__bio_for_each_segment(bvl, bio, iter, (bio)->bi_iter)

#endif /* CONFIG_BLOCK */
#endif /* __KC_LINUX_BIO_H */
