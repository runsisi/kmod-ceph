// SPDX-License-Identifier: GPL-2.0-only
/*
 * kernel/lockdep.c
 *
 * Runtime locking correctness validator
 *
 * Started by Ingo Molnar:
 *
 *  Copyright (C) 2006,2007 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2007 Red Hat, Inc., Peter Zijlstra
 *
 * this code maps all the lock dependencies as they occur in a live kernel
 * and will warn about the following classes of locking bugs:
 *
 * - lock inversion scenarios
 * - circular lock dependencies
 * - hardirq/softirq safe/unsafe locking bugs
 *
 * Bugs are reported even if the current locking scenario does not cause
 * any deadlock at this point.
 *
 * I.e. if anytime in the past two locks were taken in a different order,
 * even if it happened for another task, even if those were different
 * locks (but of the same class as this lock), this code will detect it.
 *
 * Thanks to Arjan van de Ven for coming up with the initial idea of
 * mapping lock dependencies runtime.
 */
#define DISABLE_BRANCH_PROFILING
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/sched/mm.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/interrupt.h>
#include <linux/stacktrace.h>
#include <linux/debug_locks.h>
#include <linux/irqflags.h>
#include <linux/utsname.h>
#include <linux/hash.h>
#include <linux/ftrace.h>
#include <linux/stringify.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/gfp.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/nmi.h>
#include <linux/rcupdate.h>
#include <linux/kprobes.h>

#include <asm/sections.h>

#define CREATE_TRACE_POINTS
#include <trace/events/lock.h>

static int __lock_is_held(const struct lockdep_map *lock, int read);

static nokprobe_inline
int __lock_is_held(const struct lockdep_map *lock, int read)
{
	struct task_struct *curr = current;
	int i;

	for (i = 0; i < curr->lockdep_depth; i++) {
		struct held_lock *hlock = curr->held_locks + i;

		if (match_held_lock(hlock, lock)) {
			if (read == -1 || hlock->read == read)
				return 1;

			return 0;
		}
	}

	return 0;
}

/*
 * Check whether we follow the irq-flags state precisely:
 */
static void check_flags(unsigned long flags)
{
}

int lock_is_held_type(const struct lockdep_map *lock, int read)
{
	unsigned long flags;
	int ret = 0;

	if (unlikely(current->lockdep_recursion))
		return 1; /* avoid false negative lockdep_assert_held() */

	raw_local_irq_save(flags);
	check_flags(flags);

	current->lockdep_recursion = 1;
	ret = __lock_is_held(lock, read);
	current->lockdep_recursion = 0;
	raw_local_irq_restore(flags);

	return ret;
}
//EXPORT_SYMBOL_GPL(lock_is_held_type);
NOKPROBE_SYMBOL(lock_is_held_type);
