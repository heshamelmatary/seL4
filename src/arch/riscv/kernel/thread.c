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

void
Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);

#if defined(CONFIG_IPC_BUF_IN_RISCV_TP_REGISTER)
    setRegister(tcb, tp, tcb->tcbIPCBuffer);
#else
#error "Unknown IPC buffer strategy"
#endif

}

BOOT_CODE void
Arch_configureIdleThread(tcb_t *tcb, int affinity)
{
    setRegister(tcb, SEPC, (word_t)idleThreadStart);

    /* Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) SSTATUS_SPP | SSTATUS_SPIE | SSTATUS_SIE);
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
