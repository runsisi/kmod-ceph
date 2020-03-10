/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Runtime locking correctness validator
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * see Documentation/locking/lockdep-design.rst for more details.
 */
#ifndef __KC_LINUX_LOCKDEP_H
#define __KC_LINUX_LOCKDEP_H

#include_next <linux/lockdep.h>

#ifdef CONFIG_LOCKDEP

#define lockdep_assert_held_write(l)	do {			\
		WARN_ON(debug_locks && !lockdep_is_held_type(l, 0));	\
	} while (0)

#define lockdep_assert_held_read(l)	do {				\
		WARN_ON(debug_locks && !lockdep_is_held_type(l, 1));	\
	} while (0)

#else /* !CONFIG_LOCKDEP */

#define lockdep_assert_held_write(l)	do { (void)(l); } while (0)
#define lockdep_assert_held_read(l)		do { (void)(l); } while (0)

#endif /* !LOCKDEP */

#endif /* __KC_LINUX_LOCKDEP_H */
