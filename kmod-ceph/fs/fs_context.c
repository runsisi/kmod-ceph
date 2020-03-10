// SPDX-License-Identifier: GPL-2.0-or-later
/* Provide a way to create a superblock configuration context within the kernel
 * that allows a superblock to be set up prior to mounting.
 *
 * Copyright (C) 2017 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/fs_context.h>
#include <linux/fs_parser.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/nsproxy.h>
#include <linux/slab.h>
#include <linux/magic.h>
#include <linux/security.h>
#include <linux/mnt_namespace.h>
#include <linux/pid_namespace.h>
#include <linux/user_namespace.h>
#include <net/net_namespace.h>
#include <asm/sections.h>

/**
 * logfc - Log a message to a filesystem context
 * @fc: The filesystem context to log to.
 * @fmt: The format of the buffer.
 */
void logfc(struct fc_log *log, const char *prefix, char level, const char *fmt, ...)
{
	va_list va;
	struct va_format vaf = {.fmt = fmt, .va = &va};

	va_start(va, fmt);
	if (!log) {
		switch (level) {
		case 'w':
			printk(KERN_WARNING "%s%s%pV\n", prefix ? prefix : "",
						prefix ? ": " : "", &vaf);
			break;
		case 'e':
			printk(KERN_ERR "%s%s%pV\n", prefix ? prefix : "",
						prefix ? ": " : "", &vaf);
			break;
		default:
			printk(KERN_NOTICE "%s%s%pV\n", prefix ? prefix : "",
						prefix ? ": " : "", &vaf);
			break;
		}
	} else {
		unsigned int logsize = ARRAY_SIZE(log->buffer);
		u8 index;
		char *q = kasprintf(GFP_KERNEL, "%c %s%s%pV\n", level,
						prefix ? prefix : "",
						prefix ? ": " : "", &vaf);

		index = log->head & (logsize - 1);
		BUILD_BUG_ON(sizeof(log->head) != sizeof(u8) ||
			     sizeof(log->tail) != sizeof(u8));
		if ((u8)(log->head - log->tail) == logsize) {
			/* The buffer is full, discard the oldest message */
			if (log->need_free & (1 << index))
				kfree(log->buffer[index]);
			log->tail++;
		}

		log->buffer[index] = q ? q : "OOM: Can't store error string";
		if (q)
			log->need_free |= 1 << index;
		else
			log->need_free &= ~(1 << index);
		log->head++;
	}
	va_end(va);
}
//EXPORT_SYMBOL(logfc);
