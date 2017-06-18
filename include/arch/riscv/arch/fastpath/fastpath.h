/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_FASTPATH_H
#define __ARCH_FASTPATH_H

#include <config.h>
#include <util.h>
#include <arch/linker.h>
#include <api/types.h>
#include <api/syscall.h>
#include <api/types.h>
#include <smp/lock.h>

void slowpath(syscall_t syscall)
NORETURN;

void fastpath_call(word_t cptr, word_t r_msgInfo)
VISIBLE FASTCALL NORETURN SECTION(".vectors.text");

void fastpath_reply_wait(word_t cptr, word_t r_msgInfo)
VISIBLE FASTCALL NORETURN SECTION(".vectors.text");

/* When building the fastpath the assembler in traps.S makes these
 * assumptions. Because compile_asserts are hard to do in assembler,
 * we place them here */
compile_assert(SysCall_Minus1, SysCall == -1)
compile_assert(SysReplyRecv_Minus2, SysReplyRecv == -2)

/* Use macros to not break verification */
#define endpoint_ptr_get_epQueue_tail_fp(ep_ptr) TCB_PTR(endpoint_ptr_get_epQueue_tail(ep_ptr))
#define cap_vtable_cap_get_vspace_root_fp(vtable_cap) PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(vtable_cap))

static inline void FORCE_INLINE
switchToThread_fp(tcb_t *thread, pde_t *vroot, pde_t stored_hw_asid)
{
    asid_t asid = (asid_t)(stored_hw_asid.words[0] & 0xffff);

    riscv_vm_contextSwitch(vroot, asid);

#if defined(CONFIG_IPC_BUF_GLOBALS_FRAME)
    *riscvKSGlobalsFrame = thread->tcbIPCBuffer;
#elif defined(CONFIG_IPC_BUF_IN_RISCV_TP_REGISTER)
    setRegister(thread, tp, thread->tcbIPCBuffer);
#else
#error "Unknown IPC buffer strategy"
#endif

    ksCurThread = thread;
}

static inline void
mdb_node_ptr_mset_mdbNext_mdbRevocable_mdbFirstBadged(
    mdb_node_t *node_ptr, word_t mdbNext,
    word_t mdbRevocable, word_t mdbFirstBadged)
{
    node_ptr->words[1] = mdbNext | (mdbRevocable << 1) | mdbFirstBadged;
}

static inline void
mdb_node_ptr_set_mdbPrev_np(mdb_node_t *node_ptr, word_t mdbPrev)
{
    node_ptr->words[0] = mdbPrev;
}

static inline bool_t
isValidVTableRoot_fp(cap_t vspace_root_cap)
{
    return cap_capType_equals(vspace_root_cap, cap_page_directory_cap) && cap_page_directory_cap_get_capPDIsMapped(vspace_root_cap);
}

/* This is an accelerated check that msgLength, which appears
   in the bottom of the msgInfo word, is <= 4 and that msgExtraCaps
   which appears above it is zero. We are assuming that n_msgRegisters == 4
   for this check to be useful. By masking out the bottom 3 bits, we are
   really checking that n + 3 <= MASK(3), i.e. n + 3 <= 7 or n <= 4. */
compile_assert (n_msgRegisters_eq_4, n_msgRegisters == 4)
static inline int
fastpath_mi_check(word_t msgInfo)
{
    return (msgInfo & MASK(seL4_MsgLengthBits + seL4_MsgExtraCapBits)) > 4;
}

static inline void
fastpath_copy_mrs(word_t length, tcb_t *src, tcb_t *dest)
{
    word_t i;
    register_t reg;

    /* assuming that length < n_msgRegisters */
    for (i = 0; i < length; i ++) {
        /* assuming that the message registers simply increment */
        reg = msgRegisters[0] + i;
        setRegister(dest, reg, getRegister(src, reg));
    }
}

static inline int
fastpath_reply_cap_check(cap_t cap)
{
    return cap_capType_equals(cap, cap_reply_cap);
}

/** DONT_TRANSLATE */
static inline void NORETURN
fastpath_restore(word_t badge, word_t msgInfo, tcb_t *cur_thread)
{
    NODE_UNLOCK_IF_HELD;

    //word_t cur_thread = (word_t) NODE_STATE(ksCurThread);

    c_exit_hook();

    register word_t badge_reg asm("a0") = badge;
    register word_t msgInfo_reg asm("a1") = msgInfo;
    register word_t cur_thread_reg asm("a2") = (word_t)ksCurThread->tcbArch.tcbContext.registers;


    asm volatile(
        "la t0, ksCurThread      \n"
        "ld t0, 0(t0)    \n"
        "ld x1, 33*8(t0) \n"
        "csrw sstatus, x1\n"
        "ld ra, 1*8(t0)  \n"
        "ld sp, 2*8(t0)  \n"
        "ld gp, 3*8(t0)  \n"
        /* skip tp */
        /* skip x5/t0 */
        "ld t2, 7*8(t0)  \n"
        "ld s0, 8*8(t0)  \n"
        "ld s1, 9*8(t0)  \n"
        /* Skip a0, a1; Set above, Restore MRs */
        /* "ld a0, 10*8(t0) \n" */
        /* "ld a1, 11*8(t0) \n" */
        "ld a2, 12*8(t0) \n"
        "ld a3, 13*8(t0) \n"
        "ld a4, 14*8(t0) \n"
        "ld a5, 15*8(t0) \n"
        "ld a6, 16*8(t0) \n"
        "ld a7, 17*8(t0) \n"
        "ld s2, 18*8(t0) \n"
        "ld s3, 19*8(t0) \n"
        "ld s4, 20*8(t0) \n"
        "ld s5, 21*8(t0) \n"
        "ld s6, 22*8(t0) \n"
        "ld s7, 23*8(t0) \n"
        "ld s8, 24*8(t0) \n"
        "ld s9, 25*8(t0) \n"
        "ld s10, 26*8(t0)\n"
        "ld s11, 27*8(t0)\n"
        "ld t3, 28*8(t0) \n"
        "ld t4, 29*8(t0) \n"
        "ld t5, 30*8(t0) \n"
        "ld t6, 31*8(t0) \n"
        /* Get next restored tp */
        "ld t1, 4*8(t0)  \n"
        /* get restored tp */
        "add tp, t1, x0  \n"
        /* get sepc */
        "ld t1, 34*8(t0)\n"
        "csrw sepc, t1  \n"

        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0\n"

        "ld t1, (6*8)(t0) \n"
        "ld t0, (5*8)(t0) \n"
        "sret"
        : /* no output */
        :
        : "memory"
    );
    UNREACHABLE();
}
#endif
