/*
 * Written by Mark Hemment, 1996 (markhe@nextd.demon.co.uk).
 *
 * (C) SGI 2006, Christoph Lameter
 * 	Cleaned up and restructured to ease the addition of alternative
 * 	implementations of SLAB allocators.
 */

#ifndef _KC_LINUX_SLAB_H
#define	_KC_LINUX_SLAB_H

#include_next <linux/slab.h>

#ifdef CONFIG_MEMCG_KMEM
# define SLAB_ACCOUNT		0x04000000UL	/* Account to memcg */
#else
# define SLAB_ACCOUNT		0x00000000UL
#endif

#endif	/* _KC_LINUX_SLAB_H */
