/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KC_LINUX_JIFFIES_H
#define _KC_LINUX_JIFFIES_H

#include_next <linux/jiffies.h>

extern unsigned long timespec64_to_jiffies(const struct timespec64 *value);
extern void jiffies_to_timespec64(const unsigned long jiffies,
				  struct timespec64 *value);

#endif
