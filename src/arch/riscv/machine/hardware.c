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
#include <machine/registerset.h>
#include <arch/machine.h>

word_t PURE
getRestartPC(tcb_t *thread)
{

    /* On syscall traps, the SEPC is increased by 4 to point to the next instruction,
     * subtract it back if we need to restart the syscall. 8 is the current code for syscall
     * @See RISC-V priv-1.10 manual
     */
    return getRegister(thread, SEPC);
}

void
setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NEXTPC, v);
}
