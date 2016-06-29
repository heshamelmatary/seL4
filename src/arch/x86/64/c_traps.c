/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <config.h>
#include <model/statedata.h>
#include <arch/kernel/lock.h>
#include <arch/machine/fpu.h>

#include <api/syscall.h>

extern word_t irq_stack[6];

void __attribute__((noreturn)) __attribute__((externally_visible)) restore_user_context(void);
void __attribute__((noreturn)) __attribute__((externally_visible)) restore_user_context(void)
{
    if (unlikely(ksCurThread == x86KSfpuOwner)) {
        /* We are using the FPU, make sure it is enabled */
        enableFpu();
    } else if (unlikely(x86KSfpuOwner)) {
        /* Someone is using the FPU and it might be enabled */
        disableFpu();
    } else {
        /* No-one (including us) is using the FPU, so we assume it
         * is currently disabled */
    }
    /* see if we entered via syscall */
    if (likely(ksCurThread->tcbArch.tcbContext.registers[Error] == -1)) {
        if (config_set(CONFIG_SYSENTER)) {
            ksCurThread->tcbArch.tcbContext.registers[RFLAGS] &= ~0x200;
            asm volatile(
                // Set our stack pointer to the top of the tcb so we can efficiently pop
                "movq %0, %%rsp\n"
                "popq %%rdi\n"
                "popq %%rsi\n"
                "popq %%rax\n"
                "popq %%rbx\n"
                "popq %%rbp\n"
                "popq %%r12\n"
                "popq %%r13\n"
                "popq %%r14\n"
                "popq %%r15\n"
                // skip RDX
                "addq $8, %%rsp\n"
                "popq %%r10\n"
                "popq %%r8\n"
                "popq %%r9\n"
                //restore RFLAGS
                "popfq\n"
                // reset interrupt bit
                "orq $0x200, -8(%%rsp)\n"
                // Restore NextIP
                "popq %%rdx\n"
                // Skip ERROR
                "addq $8, %%rsp\n"
                // Restore RSP
                "popq %%rcx\n"
                // Skip TLS_BASE, FaultIP
                "addq $16, %%rsp\n"
                "popq %%r11\n"
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "sti\n"
                /* return to user
                 * sysexit with rex.w user code = cs + 32, user data = cs + 40.
                 * without rex.w user code = cs + 16, user data = cs + 24
                 * */
                "rex.w sysexit\n"
                :
                : "r"(&ksCurThread->tcbArch.tcbContext.registers[RDI])
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        } else {
            asm volatile(
                // Set our stack pointer to the top of the tcb so we can efficiently pop
                "movq %0, %%rsp\n"
                "popq %%rdi\n"
                "popq %%rsi\n"
                "popq %%rax\n"
                "popq %%rbx\n"
                "popq %%rbp\n"
                "popq %%r12\n"
                "popq %%r13\n"
                "popq %%r14\n"
                "popq %%r15\n"
                "popq %%rdx\n"
                "popq %%r10\n"
                "popq %%r8\n"
                "popq %%r9\n"
                //restore RFLAGS
                "popq %%r11\n"
                // Restore NextIP
                "popq %%rcx\n"
                // clear RSP to not leak information to the user
                "xor %%rsp, %%rsp\n"
                // More register but we can ignore and are done restoring
                // enable interrupt disabled by sysenter
                "rex.w sysret\n"
                :
                : "r"(&ksCurThread->tcbArch.tcbContext.registers[RDI])
                // Clobber memory so the compiler is forced to complete all stores
                // before running this assembler
                : "memory"
            );
        }
    } else {
        /* construct our return from interrupt frame */
        irq_stack[1] = getRegister(ksCurThread, NextIP);
        irq_stack[2] = getRegister(ksCurThread, CS);
        irq_stack[3] = getRegister(ksCurThread, RFLAGS);
        irq_stack[4] = getRegister(ksCurThread, RSP);
        irq_stack[5] = getRegister(ksCurThread, SS);
        asm volatile(
            // Set our stack pointer to the top of the tcb so we can efficiently pop
            "movq %0, %%rsp\n"
            "popq %%rdi\n"
            "popq %%rsi\n"
            "popq %%rax\n"
            "popq %%rbx\n"
            "popq %%rbp\n"
            "popq %%r12\n"
            "popq %%r13\n"
            "popq %%r14\n"
            "popq %%r15\n"
            "popq %%rdx\n"
            "popq %%r10\n"
            "popq %%r8\n"
            "popq %%r9\n"
            /* skip RFLAGS, Error NextIP RSP, TLS_BASE, FaultIP */
            "addq $48, %%rsp\n"
            "popq %%r11\n"
            "popq %%rcx\n"
            "leaq irq_stack + 8, %%rsp\n"
            "iretq\n"
            :
            : "r"(&ksCurThread->tcbArch.tcbContext.registers[RDI])
            // Clobber memory so the compiler is forced to complete all stores
            // before running this assembler
            : "memory"
        );
    }
    while(1);
}

void c_handle_interrupt(int irq, int syscall);

void __attribute__((externally_visible)) c_x64_handle_interrupt(int irq, int syscall);
void __attribute__((externally_visible)) c_x64_handle_interrupt(int irq, int syscall)
{
    setRegister(ksCurThread, Error, irq_stack[0]);
    setRegister(ksCurThread, NextIP, irq_stack[1]);
    setRegister(ksCurThread, FaultIP, irq_stack[1]);
    setRegister(ksCurThread, RFLAGS, irq_stack[3]);
    setRegister(ksCurThread, RSP, irq_stack[4]);
    c_handle_interrupt(irq, syscall);
    /* does not return */
}
