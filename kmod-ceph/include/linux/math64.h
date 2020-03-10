/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KC_LINUX_MATH64_H
#define _KC_LINUX_MATH64_H

#include_next <linux/math64.h>

#define DIV64_U64_ROUND_UP(ll, d)	\
	({ u64 _tmp = (d); div64_u64((ll) + _tmp - 1, _tmp); })

#endif /* _KC_LINUX_MATH64_H */
