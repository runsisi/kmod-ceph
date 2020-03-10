// SPDX-License-Identifier: GPL-2.0-only

#include <linux/wait.h>
#include <linux/rbtree.h>
#include <linux/backing-dev.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/mm.h>
#include <linux/slab.h>

struct backing_dev_info *bdi_alloc_node(gfp_t gfp_mask, int node_id)
{
	struct backing_dev_info *bdi;

	bdi = kmalloc_node(sizeof(struct backing_dev_info),
			   gfp_mask | __GFP_ZERO, node_id);
	if (!bdi)
		return NULL;

	if (bdi_init(bdi)) {
		kfree(bdi);
		return NULL;
	}
	return bdi;
}
//EXPORT_SYMBOL(bdi_alloc_node);

int sb_is_blkdev_sb(struct super_block *sb)
{
//	return sb == blockdev_superblock;
	return false;
}
