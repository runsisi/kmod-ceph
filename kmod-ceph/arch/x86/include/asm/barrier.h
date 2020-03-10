/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KC_ASM_X86_BARRIER_H
#define _KC_ASM_X86_BARRIER_H

#include_next <asm/barrier.h>

#ifndef __ASSEMBLY__

/**
 * smp_acquire__after_ctrl_dep() - Provide ACQUIRE ordering after a control dependency
 *
 * A control dependency provides a LOAD->STORE order, the additional RMB
 * provides LOAD->LOAD order, together they provide LOAD->{LOAD,STORE} order,
 * aka. (load)-ACQUIRE.
 *
 * Architectures that do not do load speculation can have this be barrier().
 */
#ifndef smp_acquire__after_ctrl_dep
#define smp_acquire__after_ctrl_dep()		smp_rmb()
#endif

#endif /* !__ASSEMBLY__ */

#endif /* _KC_ASM_X86_BARRIER_H */
