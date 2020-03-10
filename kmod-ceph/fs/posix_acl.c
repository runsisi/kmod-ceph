// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2002,2003 by Andreas Gruenbacher <a.gruenbacher@computer.org>
 *
 * Fixes from William Schumacher incorporated on 15 March 2001.
 *    (Reported by Charles Bertsch, <CBertsch@microtest.com>).
 */

/*
 *  This file contains generic functions for manipulating
 *  POSIX 1003.1e draft standard 17 ACLs.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/posix_acl.h>
#include <linux/posix_acl_xattr.h>
#include <linux/xattr.h>
#include <linux/export.h>
#include <linux/user_namespace.h>

/*
 * Clone an ACL.
 */
static struct posix_acl *
posix_acl_clone(const struct posix_acl *acl, gfp_t flags)
{
	struct posix_acl *clone = NULL;

	if (acl) {
		int size = sizeof(struct posix_acl) + acl->a_count *
		           sizeof(struct posix_acl_entry);
		clone = kmemdup(acl, size, flags);
		if (clone)
			atomic_set(&clone->a_refcount, 1);
	}
	return clone;
}

/*
 * Modify acl when creating a new inode. The caller must ensure the acl is
 * only referenced once.
 *
 * mode_p initially must contain the mode parameter to the open() / creat()
 * system calls. All permissions that are not granted by the acl are removed.
 * The permissions in the acl are changed to reflect the mode_p parameter.
 */
static int posix_acl_create_masq(struct posix_acl *acl, umode_t *mode_p)
{
	struct posix_acl_entry *pa, *pe;
	struct posix_acl_entry *group_obj = NULL, *mask_obj = NULL;
	umode_t mode = *mode_p;
	int not_equiv = 0;

	/* assert(atomic_read(acl->a_refcount) == 1); */

	FOREACH_ACL_ENTRY(pa, acl, pe) {
                switch(pa->e_tag) {
                        case ACL_USER_OBJ:
				pa->e_perm &= (mode >> 6) | ~S_IRWXO;
				mode &= (pa->e_perm << 6) | ~S_IRWXU;
				break;

			case ACL_USER:
			case ACL_GROUP:
				not_equiv = 1;
				break;

                        case ACL_GROUP_OBJ:
				group_obj = pa;
                                break;

                        case ACL_OTHER:
				pa->e_perm &= mode | ~S_IRWXO;
				mode &= pa->e_perm | ~S_IRWXO;
                                break;

                        case ACL_MASK:
				mask_obj = pa;
				not_equiv = 1;
                                break;

			default:
				return -EIO;
                }
        }

	if (mask_obj) {
		mask_obj->e_perm &= (mode >> 3) | ~S_IRWXO;
		mode &= (mask_obj->e_perm << 3) | ~S_IRWXG;
	} else {
		if (!group_obj)
			return -EIO;
		group_obj->e_perm &= (mode >> 3) | ~S_IRWXO;
		mode &= (group_obj->e_perm << 3) | ~S_IRWXG;
	}

	*mode_p = (*mode_p & ~S_IRWXUGO) | mode;
        return not_equiv;
}

int
kc_posix_acl_create(struct inode *dir, umode_t *mode,
		struct posix_acl **default_acl, struct posix_acl **acl)
{
	struct posix_acl *p;
	struct posix_acl *clone;
	int ret;

	*acl = NULL;
	*default_acl = NULL;

	if (S_ISLNK(*mode) || !IS_POSIXACL(dir))
		return 0;

	p = get_acl(dir, ACL_TYPE_DEFAULT);
	if (!p || p == ERR_PTR(-EOPNOTSUPP)) {
		*mode &= ~current_umask();
		return 0;
	}
	if (IS_ERR(p))
		return PTR_ERR(p);

	ret = -ENOMEM;
	clone = posix_acl_clone(p, GFP_NOFS);
	if (!clone)
		goto err_release;

	ret = posix_acl_create_masq(clone, mode);
	if (ret < 0)
		goto err_release_clone;

	if (ret == 0)
		posix_acl_release(clone);
	else
		*acl = clone;

	if (!S_ISDIR(*mode))
		posix_acl_release(p);
	else
		*default_acl = p;

	return 0;

err_release_clone:
	posix_acl_release(clone);
err_release:
	posix_acl_release(p);
	return ret;
}
//EXPORT_SYMBOL_GPL(posix_acl_create);
