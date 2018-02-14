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

#ifndef __ARCH_FASTPATH_H
#define __ARCH_FASTPATH_H

#include <config.h>
#include <util.h>
#include <linker.h>
#include <api/types.h>
#include <api/syscall.h>
#include <api/types.h>
#include <smp/lock.h>
#include <arch/machine/hardware.h>

void slowpath(syscall_t syscall)
NORETURN;

void fastpath_call(word_t cptr, word_t r_msgInfo)
VISIBLE NORETURN SECTION(".vectors.text");

void fastpath_reply_wait(word_t cptr, word_t r_msgInfo)
VISIBLE NORETURN SECTION(".vectors.text");

/* Use macros to not break verification */
#define endpoint_ptr_get_epQueue_tail_fp(ep_ptr) TCB_PTR(endpoint_ptr_get_epQueue_tail(ep_ptr))
#define cap_vtable_cap_get_vspace_root_fp(vtable_cap) PTE_PTR(cap_page_table_cap_get_capPTBasePtr(vtable_cap))

static inline void FORCE_INLINE
switchToThread_fp(tcb_t *thread, pte_t *vroot, pte_t stored_hw_asid)
{
    asid_t asid = (asid_t)(stored_hw_asid.words[0]);

    setVSpaceRoot(vroot, asid);

    setRegister(thread, tp, thread->tcbIPCBuffer);

    NODE_STATE(ksCurThread) = thread;
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
    return cap_capType_equals(vspace_root_cap, cap_page_table_cap) &&
           cap_page_table_cap_get_capPTIsMapped(vspace_root_cap);
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

    c_exit_hook();

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(NODE_STATE(ksCurThread));
#endif /* CONFIG_HAVE_FPU */

    register word_t badge_reg asm("a0") = badge;
    register word_t msgInfo_reg asm("a1") = msgInfo;
    register word_t cur_thread_reg asm("a2") = (word_t) NODE_STATE(ksCurThread)->tcbArch.tcbContext.registers;

    asm volatile(
        "mv t0, %[cur_thread]    \n"

        "mv a0, %[badge] \n"
        "mv a1, %[info] \n"

        LOAD_S "  ra, (0*%[REGSIZE])(t0)  \n"
        LOAD_S "  sp, (1*%[REGSIZE])(t0)  \n"
        LOAD_S "  gp, (2*%[REGSIZE])(t0)  \n"
        /* skip tp */
        /* skip x5/t0 */
        LOAD_S "  t2, (6*%[REGSIZE])(t0)  \n"
        LOAD_S "  s0, (7*%[REGSIZE])(t0)  \n"
        LOAD_S "  s1, (8*%[REGSIZE])(t0)  \n"
        LOAD_S "  a2, (11*%[REGSIZE])(t0) \n"
        LOAD_S "  a3, (12*%[REGSIZE])(t0) \n"
        LOAD_S "  a4, (13*%[REGSIZE])(t0) \n"
        LOAD_S "  a5, (14*%[REGSIZE])(t0) \n"
        LOAD_S "  a6, (15*%[REGSIZE])(t0) \n"
        LOAD_S "  a7, (16*%[REGSIZE])(t0) \n"
        LOAD_S "  s2, (17*%[REGSIZE])(t0) \n"
        LOAD_S "  s3, 18*%[REGSIZE](t0) \n"
        LOAD_S "  s4, 19*%[REGSIZE](t0) \n"
        LOAD_S "  s5, 20*%[REGSIZE](t0) \n"
        LOAD_S "  s6, 21*%[REGSIZE](t0) \n"
        LOAD_S "  s7, 22*%[REGSIZE](t0) \n"
        LOAD_S "  s8, 23*%[REGSIZE](t0) \n"
        LOAD_S "  s9, 24*%[REGSIZE](t0) \n"
        LOAD_S "  s10, 25*%[REGSIZE](t0)\n"
        LOAD_S "  s11, 26*%[REGSIZE](t0)\n"
        LOAD_S "  t3, 27*%[REGSIZE](t0) \n"
        LOAD_S "  t4, 28*%[REGSIZE](t0) \n"
        LOAD_S "  t5, 29*%[REGSIZE](t0) \n"
        LOAD_S "  t6, 30*%[REGSIZE](t0) \n"
        /* Get next restored tp */
        LOAD_S "  t1, 3*%[REGSIZE](t0)  \n"
        /* get restored tp */
        "add tp, t1, x0  \n"
        /* get sepc */
        LOAD_S "  t1, 35*%[REGSIZE](t0)\n"
        "csrw sepc, t1  \n"

        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0\n"

        LOAD_S "  t1, 32*%[REGSIZE](t0) \n"
        "csrw sstatus, t1\n"

        LOAD_S "  t1, (5*%[REGSIZE])(t0) \n"
        LOAD_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [cur_thread] "r" (cur_thread_reg),
        [REGSIZE] "i" (sizeof(word_t)),
        [badge] "r" (badge),
        [info]  "r" (msgInfo)
        : "memory", "t0"
    );

    UNREACHABLE();
}
#endif
