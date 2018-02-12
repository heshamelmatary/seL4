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

#ifndef __ARCH_MACHINE_FPU_H
#define __ARCH_MACHINE_FPU_H

#include <machine/registerset.h>
#include <util.h>

#ifdef CONFIG_HAVE_FPU
extern bool_t isFPUEnabledCached[CONFIG_MAX_NUM_NODES];

static inline bool_t isDirtyFPU(void)
{
    return (read_csr(sstatus) & BIT(CONFIG_WORD_SIZE - 1));
}

static inline bool_t cleanFPUState(void)
{
    word_t sstatus = NODE_STATE(ksCurThread)->tcbArch.tcbContext.registers[SSTATUS] ;
    sstatus = (sstatus & ~SSTATUS_FS) | SSTATUS_FS_CLEAN;
    NODE_STATE(ksCurThread)->tcbArch.tcbContext.registers[SSTATUS] = sstatus;
}

/* Store state in the FPU registers into memory. */
static inline void saveFpuState(user_fpu_state_t *dest)
{

    if (isDirtyFPU()) {

        /* Save FPU state */
        asm volatile(
            "fsd f0, (0*%[FREGSIZE])(%[fpregs])\n"
            "fsd f1, (1*%[FREGSIZE])(%[fpregs])\n"
            "fsd f2, (2*%[FREGSIZE])(%[fpregs])\n"
            "fsd f3, (3*%[FREGSIZE])(%[fpregs])\n"
            "fsd f4, (4*%[FREGSIZE])(%[fpregs])\n"
            "fsd f5, (5*%[FREGSIZE])(%[fpregs])\n"
            "fsd f6, (6*%[FREGSIZE])(%[fpregs])\n"
            "fsd f7, (7*%[FREGSIZE])(%[fpregs])\n"
            "fsd f8, (8*%[FREGSIZE])(%[fpregs])\n"
            "fsd f9, (9*%[FREGSIZE])(%[fpregs])\n"
            "fsd f10, (10*%[FREGSIZE])(%[fpregs])\n"
            "fsd f11, (11*%[FREGSIZE])(%[fpregs])\n"
            "fsd f12, (12*%[FREGSIZE])(%[fpregs])\n"
            "fsd f13, (13*%[FREGSIZE])(%[fpregs])\n"
            "fsd f14, (14*%[FREGSIZE])(%[fpregs])\n"
            "fsd f15, (15*%[FREGSIZE])(%[fpregs])\n"
            "fsd f16, (16*%[FREGSIZE])(%[fpregs])\n"
            "fsd f17, (17*%[FREGSIZE])(%[fpregs])\n"
            "fsd f18, (18*%[FREGSIZE])(%[fpregs])\n"
            "fsd f19, (19*%[FREGSIZE])(%[fpregs])\n"
            "fsd f20, (20*%[FREGSIZE])(%[fpregs])\n"
            "fsd f21, (21*%[FREGSIZE])(%[fpregs])\n"
            "fsd f22, (22*%[FREGSIZE])(%[fpregs])\n"
            "fsd f23, (23*%[FREGSIZE])(%[fpregs])\n"
            "fsd f24, (24*%[FREGSIZE])(%[fpregs])\n"
            "fsd f25, (25*%[FREGSIZE])(%[fpregs])\n"
            "fsd f26, (26*%[FREGSIZE])(%[fpregs])\n"
            "fsd f27, (27*%[FREGSIZE])(%[fpregs])\n"
            "fsd f28, (28*%[FREGSIZE])(%[fpregs])\n"
            "fsd f29, (29*%[FREGSIZE])(%[fpregs])\n"
            "fsd f30, (30*%[FREGSIZE])(%[fpregs])\n"
            "fsd f31, (31*%[FREGSIZE])(%[fpregs])\n"

            //"FSCSR %[temp_fcsr] 0(%[fcsr])\n"
            :
            : [fpregs] "r" (&dest->fpregs[0]),
              [fcsr] "r" (&dest->fcsr),
              [FREGSIZE] "i" (sizeof(dest->fpregs[0]))
            : "memory"
        );

        cleanFPUState();
    }

}

/* Enable the FPU to be used without faulting.
 * Required even if the kernel attempts to use the FPU. */
static inline void enableFpu(void)
{
    set_csr(sstatus, SSTATUS_FS_CLEAN);
    cleanFPUState();
}

/* Check if FPU is enable */
static inline bool_t isFpuEnable(void)
{
    return !!(read_csr(sstatus) & SSTATUS_FS);
}

/* Load FPU state from memory into the FPU registers. */
static inline void loadFpuState(user_fpu_state_t *src)
{

    /* FPU is not off */
    if (!isDirtyFPU) {
        /* load */
        asm volatile(
            "fld f0, (0*%[FREGSIZE])(%[fpregs])\n"
            "fld f1, (1*%[FREGSIZE])(%[fpregs])\n"
            "fld f2, (2*%[FREGSIZE])(%[fpregs])\n"
            "fld f3, (3*%[FREGSIZE])(%[fpregs])\n"
            "fld f4, (4*%[FREGSIZE])(%[fpregs])\n"
            "fld f5, (5*%[FREGSIZE])(%[fpregs])\n"
            "fld f6, (6*%[FREGSIZE])(%[fpregs])\n"
            "fld f7, (7*%[FREGSIZE])(%[fpregs])\n"
            "fld f8, (8*%[FREGSIZE])(%[fpregs])\n"
            "fld f9, (9*%[FREGSIZE])(%[fpregs])\n"
            "fld f10, (10*%[FREGSIZE])(%[fpregs])\n"
            "fld f11, (11*%[FREGSIZE])(%[fpregs])\n"
            "fld f12, (12*%[FREGSIZE])(%[fpregs])\n"
            "fld f13, (13*%[FREGSIZE])(%[fpregs])\n"
            "fld f14, (14*%[FREGSIZE])(%[fpregs])\n"
            "fld f15, (15*%[FREGSIZE])(%[fpregs])\n"
            "fld f16, (16*%[FREGSIZE])(%[fpregs])\n"
            "fld f17, (17*%[FREGSIZE])(%[fpregs])\n"
            "fld f18, (18*%[FREGSIZE])(%[fpregs])\n"
            "fld f19, (19*%[FREGSIZE])(%[fpregs])\n"
            "fld f20, (20*%[FREGSIZE])(%[fpregs])\n"
            "fld f21, (21*%[FREGSIZE])(%[fpregs])\n"
            "fld f22, (22*%[FREGSIZE])(%[fpregs])\n"
            "fld f23, (23*%[FREGSIZE])(%[fpregs])\n"
            "fld f24, (24*%[FREGSIZE])(%[fpregs])\n"
            "fld f25, (25*%[FREGSIZE])(%[fpregs])\n"
            "fld f26, (26*%[FREGSIZE])(%[fpregs])\n"
            "fld f27, (27*%[FREGSIZE])(%[fpregs])\n"
            "fld f28, (28*%[FREGSIZE])(%[fpregs])\n"
            "fld f29, (29*%[FREGSIZE])(%[fpregs])\n"
            "fld f30, (30*%[FREGSIZE])(%[fpregs])\n"
            "fld f31, (31*%[FREGSIZE])(%[fpregs])\n"

            //"FLCSR 0(%[fcsr])\n"
            :
            : [fpregs] "r" (&src->fpregs[0]),
              [fcsr] "r" (&src->fcsr),
              [FREGSIZE] "i" (sizeof(src->fpregs[0]))
        );
        /* set fpu to clean state */
        cleanFPUState();
    } else {
        printf("That's wrong, FPU restore should never be dirty\n");
    }
}

#endif /* CONFIG_HAVE_FPU */

/* Disable the FPU so that usage of it causes a fault */
static inline void disableFpu(void)
{
    word_t sstatus = NODE_STATE(ksCurThread)->tcbArch.tcbContext.registers[SSTATUS];
    sstatus = (sstatus & ~SSTATUS_FS);
    NODE_STATE(ksCurThread)->tcbArch.tcbContext.registers[SSTATUS] = sstatus;

    write_csr(sstatus, sstatus);
}
#endif /* __ARCH_MACHINE_FPU_H */
