// SPDX-License-Identifier: GPL-2.0-only
/*
 * fs/dcache.c
 *
 * Complete reimplementation
 * (C) 1997 Thomas Schoebel-Theuer,
 * with heavy changes by Linus Torvalds
 */

/*
 * Notes on the allocation strategy:
 *
 * The dcache is a master of the icache - whenever a dcache entry
 * exists, the inode will always exist. "iput()" is done either when
 * the dcache entry is deleted or garbage collected.
 */

#include <linux/ratelimit.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/fs.h>

/**
 * d_obtain_root - find or allocate a dentry for a given inode
 * @inode: inode to allocate the dentry for
 *
 * Obtain an IS_ROOT dentry for the root of a filesystem.
 *
 * We must ensure that directory inodes only ever have one dentry.  If a
 * dentry is found, that is returned instead of allocating a new one.
 *
 * On successful return, the reference to the inode has been transferred
 * to the dentry.  In case of an error the reference on the inode is
 * released.  A %NULL or IS_ERR inode may be passed in and will be the
 * error will be propagate to the return value, with a %NULL @inode
 * replaced by ERR_PTR(-ESTALE).
 */
struct dentry *d_obtain_root(struct inode *inode)
{
	struct dentry *dentry;

	dentry = d_obtain_alias(inode);
	if (!IS_ERR(dentry)) {
		spin_lock(&dentry->d_lock);
		dentry->d_flags &= ~DCACHE_DISCONNECTED;
		spin_unlock(&dentry->d_lock);
	}
	return dentry;
}
//EXPORT_SYMBOL(d_obtain_root);
