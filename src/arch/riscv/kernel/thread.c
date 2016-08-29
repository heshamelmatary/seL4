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
#include <arch/linker.h>

void
Arch_switchToThread(tcb_t *tcb)
{
    setVMRoot(tcb);

    /* If this is the first time to run the thread, set SEPC to ra (entry point)
     * this is a sepcial case where this next task is getting runnable (context switch).
     *
     * Floating point has to be enabled also for the first-time-to-run thread
     */
    if (tcb->tcbArch.tcbContext.registers[SEPC] == 0) {
        setRegister(tcb, SEPC, tcb->tcbArch.tcbContext.registers[ra]);
        setRegister(tcb, SSTATUS, (word_t) SSTATUS_FS);
    }
    *riscvKSGlobalsFrame = tcb->tcbIPCBuffer;
}

BOOT_CODE void
Arch_configureIdleThread(tcb_t *tcb)
{
    setRegister(tcb, SEPC, (word_t)idleThreadStart);

    /* FIXME: don't use magic numbers
     * Enable interrupts and keep working in supervisor mode */
    setRegister(tcb, SSTATUS, (word_t) 0x122);
}

void
Arch_switchToIdleThread(void)
{
    *riscvKSGlobalsFrame = 0;
}

void CONST
Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}
