/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <object.h>
#include <machine.h>
#include <arch/model/statedata.h>
#include <arch/kernel/vspace.h>
#include <arch/kernel/thread.h>
#include <linker.h>

extern char kernel_stack_alloc[CONFIG_MAX_NUM_NODES][BIT(CONFIG_KERNEL_STACK_BITS)];

void
Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);
    setRegister(tcb, tp, tcb->tcbIPCBuffer);
}

BOOT_CODE void
Arch_configureIdleThread(tcb_t *tcb, int affinity)
{
    setRegister(tcb, NEXTPC, (word_t)idleThreadStart);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE | SSTATUS_SIE);
    setRegister(tcb, SP, kernel_stack_alloc);
    setRegister(tcb, HARTID, affinity);
}

void
Arch_switchToIdleThread(void)
{
    tcb_t *tcb = NODE_STATE(ksIdleThread);

    /* Force the idle thread to run on kernel page table */
    setVMRoot(tcb);
}

void CONST
Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

void
Arch_postModifyRegisters(tcb_t *tptr)
{
    /* Nothing to do */
}
