/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KC_LINUX_KERNEL_H
#define _KC_LINUX_KERNEL_H

#include_next <linux/kernel.h>

#define DIV_ROUND_DOWN_ULL(ll, d) \
	({ unsigned long long _tmp = (ll); do_div(_tmp, d); _tmp; })

#endif
