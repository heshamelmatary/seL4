/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#include <config.h>
#include <model/statedata.h>
#include <arch/kernel/traps.h>
#include <machine/debug.h>
#include <api/syscall.h>
#include <util.h>
#include <arch/machine/hardware.h>

#include <benchmark/benchmark_track.h>
#include <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
void VISIBLE NORETURN restore_user_context(void)
{
    word_t cur_thread_reg = (word_t) NODE_STATE(ksCurThread)->tcbArch.tcbContext.registers;

    c_exit_hook();

    NODE_UNLOCK_IF_HELD;

    asm volatile(
        "mv t0, %[cur_thread]       \n"
        "LW ra, (0*%[REGSIZE])(t0)  \n"
        "LW sp, (1*%[REGSIZE])(t0)  \n"
        "LW gp, (2*%[REGSIZE])(t0)  \n"
        /* skip tp */
        /* skip x5/t0 */
        "LW t2, (6*%[REGSIZE])(t0)  \n"
        "LW s0, (7*%[REGSIZE])(t0)  \n"
        "LW s1, (8*%[REGSIZE])(t0)  \n"
        "LW a0, (9*%[REGSIZE])(t0) \n"
        "LW a1, (10*%[REGSIZE])(t0) \n"
        "LW a2, (11*%[REGSIZE])(t0) \n"
        "LW a3, (12*%[REGSIZE])(t0) \n"
        "LW a4, (13*%[REGSIZE])(t0) \n"
        "LW a5, (14*%[REGSIZE])(t0) \n"
        "LW a6, (15*%[REGSIZE])(t0) \n"
        "LW a7, (16*%[REGSIZE])(t0) \n"
        "LW s2, (17*%[REGSIZE])(t0) \n"
        "LW s3, (18*%[REGSIZE])(t0) \n"
        "LW s4, (19*%[REGSIZE])(t0) \n"
        "LW s5, (20*%[REGSIZE])(t0) \n"
        "LW s6, (21*%[REGSIZE])(t0) \n"
        "LW s7, (22*%[REGSIZE])(t0) \n"
        "LW s8, (23*%[REGSIZE])(t0) \n"
        "LW s9, (24*%[REGSIZE])(t0) \n"
        "LW s10, (25*%[REGSIZE])(t0)\n"
        "LW s11, (26*%[REGSIZE])(t0)\n"
        "LW t3, (27*%[REGSIZE])(t0) \n"
        "LW t4, (28*%[REGSIZE])(t0) \n"
        "LW t5, (29*%[REGSIZE])(t0) \n"
        "LW t6, (30*%[REGSIZE])(t0) \n"
        /* Get next restored tp */
        "LW t1, (3*%[REGSIZE])(t0)  \n"
        /* get restored tp */
        "add tp, t1, x0  \n"
        /* get sepc */
        "LW t1, (35*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"

        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"

        "LW t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        "LW t1, (5*%[REGSIZE])(t0) \n"
        "LW t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i" (sizeof(word_t)),
        [cur_thread] "r" (cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

void VISIBLE NORETURN
c_handle_interrupt(void)
{
    NODE_LOCK_IRQ;

    c_entry_hook();

    handleInterruptEntry();

    restore_user_context();
    UNREACHABLE();
}

void VISIBLE NORETURN
c_handle_exception(void)
{
    NODE_LOCK_SYS;

    c_entry_hook();

    handle_exception();

    restore_user_context();
    UNREACHABLE();
}

void NORETURN
slowpath(syscall_t syscall)
{
    /* check for undefined syscall */
    if (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
        /* TODO: benchmarks - this is only added for future benchmark support
         * doesn't do anything now, and benchmarking support is currently broken,
         * and not tested.
         */
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
        /* ksKernelEntry.word word is already set to syscall */
#endif /* TRACK_KERNEL_ENTRIES */
        handleUnknownSyscall(syscall);
    } else {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

void VISIBLE NORETURN
c_handle_syscall(word_t cptr, word_t msgInfo, word_t unused1, word_t unused2, word_t unused3, word_t unused4, word_t unused5, syscall_t syscall)
{
    NODE_LOCK_SYS;

    c_entry_hook();

    /* TODO: benchmarks - this is only added for future benchmark support
     * doesn't do anything now, and benchmarking support is currently broken,
     * and not tested.
     */
#ifdef TRACK_KERNEL_ENTRIES
    //benchmark_debug_syscall_start(cptr, msgInfo, syscall);
    ksKernelEntry.is_fastpath = 1;
#endif /* TRACK_KERNEL_ENTRIES */

#ifdef CONFIG_FASTPATH
    if (syscall == (syscall_t)SysCall) {
        fastpath_call(cptr, msgInfo);
        UNREACHABLE();
    } else if (syscall == (syscall_t)SysReplyRecv) {
        fastpath_reply_recv(cptr, msgInfo);
        UNREACHABLE();
    }
#endif /* CONFIG_FASTPATH */
    slowpath(syscall);
    UNREACHABLE();
}
