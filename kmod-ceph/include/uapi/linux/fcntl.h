/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _KC_UAPI_LINUX_FCNTL_H
#define _KC_UAPI_LINUX_FCNTL_H

#include_next <uapi/linux/fcntl.h>

#define AT_STATX_SYNC_TYPE	0x6000	/* Type of synchronisation required from statx() */
#define AT_STATX_SYNC_AS_STAT	0x0000	/* - Do whatever stat() does */
#define AT_STATX_FORCE_SYNC	0x2000	/* - Force the attributes to be sync'd with the server */
#define AT_STATX_DONT_SYNC	0x4000	/* - Don't sync attributes with the server */

#endif /* _KC_UAPI_LINUX_FCNTL_H */
