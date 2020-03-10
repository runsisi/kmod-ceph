/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KC_LINUX_BLKDEV_H
#define _KC_LINUX_BLKDEV_H

#include_next <linux/blkdev.h>

#ifdef CONFIG_BLOCK

void blk_queue_flag_set(unsigned int flag, struct request_queue *q);
void blk_queue_flag_clear(unsigned int flag, struct request_queue *q);
bool blk_queue_flag_test_and_set(unsigned int flag, struct request_queue *q);

int blk_status_to_errno(blk_status_t status);
blk_status_t errno_to_blk_status(int errno);

/*
 * The basic unit of block I/O is a sector. It is used in a number of contexts
 * in Linux (blk, bio, genhd). The size of one sector is 512 = 2**9
 * bytes. Variables of type sector_t represent an offset or size that is a
 * multiple of 512 bytes. Hence these two constants.
 */
#ifndef SECTOR_SHIFT
#define SECTOR_SHIFT 9
#endif
#ifndef SECTOR_SIZE
#define SECTOR_SIZE (1 << SECTOR_SHIFT)
#endif

#endif /* CONFIG_BLOCK */

#endif
