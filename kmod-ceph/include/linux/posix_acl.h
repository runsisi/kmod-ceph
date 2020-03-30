/* SPDX-License-Identifier: GPL-2.0 */
/*
  File: linux/posix_acl.h

  (C) 2002 Andreas Gruenbacher, <a.gruenbacher@computer.org>
*/


#ifndef __KC_LINUX_POSIX_ACL_H
#define __KC_LINUX_POSIX_ACL_H

#include_next <linux/posix_acl.h>

/* posix_acl.c */

#ifdef CONFIG_FS_POSIX_ACL
extern int kc_posix_acl_create(struct inode *, umode_t *, struct posix_acl **,
		struct posix_acl **);
#else
static inline int kc_posix_acl_create(struct inode *inode, umode_t *mode,
		struct posix_acl **default_acl, struct posix_acl **acl)
{
	*default_acl = *acl = NULL;
	return 0;
}
#endif /* CONFIG_FS_POSIX_ACL */

#endif  /* __KC_LINUX_POSIX_ACL_H */
