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

//extern char kernel_stack_alloc[];
extern word_t irq_stack[6];

void
Arch_switchToThread(tcb_t* tcb)
{
    word_t base;
    /* set PD */
    setVMRoot(tcb);
    /* update the GDT_TLS entry with the thread's TLS_BASE address */
    base = getRegister(tcb, TLS_BASE);
    x86_write_fs_base(base);

    /* update the GDT_IPCBUF entry with the thread's IPC buffer address */
    base = tcb->tcbIPCBuffer;
    x86_write_gs_base(base);
}

BOOT_CODE void
Arch_configureIdleThread(tcb_t* tcb)
{
    setRegister(tcb, RFLAGS, BIT(9) | BIT(1));
    setRegister(tcb, NextIP, (uint64_t)idleThreadStart);
    setRegister(tcb, CS, SEL_CS_0);
    setRegister(tcb, SS, SEL_DS_0);
    /* In 64-bit mode the CPU unconditionally pushes to the stack when
     * taking an exception. Therefore we need to provide the idle thread
     * with a stack */
    setRegister(tcb, RSP, (uint64_t)&irq_stack[6]);
}

void
Arch_switchToIdleThread(void)
{
    /* Don't need to do anything */
}

void CONST
Arch_activateIdleThread(tcb_t* tcb)
{
    /* Don't need to do anything */
}
