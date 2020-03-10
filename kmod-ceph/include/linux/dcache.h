/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __KC_LINUX_DCACHE_H
#define __KC_LINUX_DCACHE_H

#include_next <linux/dcache.h>

extern struct dentry * d_obtain_root(struct inode *);

#endif	/* __KC_LINUX_DCACHE_H */
