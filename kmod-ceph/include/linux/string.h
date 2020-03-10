/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KC_LINUX_STRING_H_
#define _KC_LINUX_STRING_H_

#include_next <linux/string.h>

extern char *kmemdup_nul(const char *s, size_t len, gfp_t gfp);

#endif /* _KC_LINUX_STRING_H_ */
