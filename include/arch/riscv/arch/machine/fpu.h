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

#ifndef __MACHINE_FPU_H
#define __MACHINE_FPU_H

#include <mode/machine/registerset.h>
#include <util.h>

#ifdef CONFIG_HAVE_FPU
extern bool_t isFPUEnabledCached[CONFIG_MAX_NUM_NODES];

/* Store state in the FPU registers into memory. */
static inline void saveFpuState(user_fpu_state_t *dest)
{
    word_t fpexc;

    /* Store FPEXC */
    MRC(FPEXC, fpexc);

#if defined(CONFIG_ARM_CORTEX_A7) || defined(CONFIG_ARM_CORTEX_A9)
    /*
    * Reset DEX bit to 0 in case a subarchitecture sets it.
    * For example, Cortex-A7/A9 set this bit on deprecated vector VFP operations.
    */
    if (unlikely(fpexc & BIT(FPEXC_DEX_BIT))) {
        fpexc &= ~BIT(FPEXC_DEX_BIT);
        MCR(FPEXC, fpexc);
    }
#endif

    dest->fpexc = fpexc;

    /* We don't support asynchronous exceptions */
    assert ((dest->fpexc & BIT(FPEXC_EX_BIT)) == 0);

    if (isFPUD32SupportedCached) {
        register word_t regs_d16_d31 asm("ip") = (word_t) &dest->fpregs[16];
        asm volatile(
            ".word 0xeccc0b20        \n"    /*  vstmia  ip, {d16-d31} */
            :
            : "r" (regs_d16_d31)
            : "memory"
        );
    }

    register word_t regs_d0_d15 asm("r2") =  (word_t) &dest->fpregs[0];
    asm volatile(
        /* Store d0 - d15 to memory */
        ".word 0xec820b20       \n" /* vstmia  r2, {d0-d15}" */
        /* Store PFSCR */
        ".word 0xeef1ea10       \n" /* vmrs   lr, fpscr */
        "str  lr, [%[tcb_fpscr]]\n"
        :
        : [tcb_fpscr] "r" (&dest->fpscr), "r" (regs_d0_d15)
        : "memory", "lr"
    );
}

/* Enable the FPU to be used without faulting.
 * Required even if the kernel attempts to use the FPU. */
static inline void enableFpu(void)
{
    set_csr(sstatus, SSTATUS_FS);
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
    if ((read_csr(sstatus) & SSTATUS_FS)) {
        /* load */
        /* set fpu to clean state */
    }

    register word_t regs_d16_d31 asm("r2") =  (word_t) &src->fpregs[16];
    if (isFPUD32SupportedCached) {
        asm volatile(
            ".word 0xecd20b20       \n" /*   vldmia  r2, {d16-d31} */
            :: "r" (regs_d16_d31)
        );
    }

    register word_t regs_d0_d15 asm("r0") =  (word_t) &src->fpregs[0];
    register word_t regs_fpscr asm("r1") = src->fpscr;
    asm volatile(
        /* Restore d0 - d15 from memory */
        ".word 0xec900b20         \n"    /*  vldmia  r0, {d0-d15} */
        /* Load fpscr */
        ".word 0xeee11a10         \n"    /*  vmsr    fpscr, r1 */
        :: "r" (regs_d0_d15), "r" (regs_fpscr)
    );

    /* Restore FPEXC */
    MCR(FPEXC, src->fpexc);
}

#endif /* CONFIG_HAVE_FPU */

/* Disable the FPU so that usage of it causes a fault */
static inline void disableFpu(void)
{
    clear_csr(sstatus, SSTATUS_FS);
}
#endif /* __MACHINE_FPU_H */
