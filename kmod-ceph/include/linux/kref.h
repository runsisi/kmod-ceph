/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * kref.h - library routines for handling generic reference counted objects
 *
 * Copyright (C) 2004 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2004 IBM Corp.
 *
 * based on kobject.h which was:
 * Copyright (C) 2002-2003 Patrick Mochel <mochel@osdl.org>
 * Copyright (C) 2002-2003 Open Source Development Labs
 */

#ifndef _KC_KREF_H_
#define _KC_KREF_H_

#include_next <linux/kref.h>

static inline int kref_read(const struct kref *kref)
{
	return atomic_read(&kref->refcount);
}

#endif /* _KC_KREF_H_ */
