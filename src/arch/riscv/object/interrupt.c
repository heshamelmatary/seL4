/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <types.h>
#include <api/failures.h>

#include <arch/object/interrupt.h>

exception_t
Arch_decodeInterruptControl(unsigned int length, extra_caps_t extraCaps)
{
    return EXCEPTION_SYSCALL_ERROR;
}

exception_t
Arch_checkIRQ(word_t irq)
{
    return EXCEPTION_SYSCALL_ERROR;
}

exception_t
Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                cte_t *srcSlot, extra_caps_t excaps,
                                word_t *buffer)
{
    current_syscall_error.type = seL4_IllegalOperation;
    return EXCEPTION_SYSCALL_ERROR;
}
