/* SPDX-License-Identifier: GPL-2.0 */
/*
 * include/linux/backing-dev.h
 *
 * low-level device information and state which is propagated up through
 * to high-level code.
 */

#ifndef _KC_LINUX_BACKING_DEV_H
#define _KC_LINUX_BACKING_DEV_H

#include_next <linux/backing-dev.h>

struct backing_dev_info *bdi_alloc_node(gfp_t gfp_mask, int node_id);
static inline struct backing_dev_info *bdi_alloc(gfp_t gfp_mask)
{
	return bdi_alloc_node(gfp_mask, NUMA_NO_NODE);
}

#endif	/* _KC_LINUX_BACKING_DEV_H */
