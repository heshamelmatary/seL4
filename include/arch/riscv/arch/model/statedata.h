/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_MODEL_STATEDATA_H
#define __ARCH_MODEL_STATEDATA_H

#include <config.h>
#include <types.h>
#include <util.h>
#include <object/structures.h>
#include <arch/types.h>
#include <plat/machine/devices.h>

extern word_t riscvKSGlobalsFrame[BIT(PAGE_BITS) / sizeof(word_t)] ALIGN_BSS(BIT(PAGE_BITS));
extern asid_pool_t *riscvKSASIDTable[BIT(asidHighBits)];
extern char riscv_kernel_stack[BIT(RISCV_4K_PageBits)] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
extern pde_t l1pt[512] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
extern pte_t l2pt[512] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
extern pte_t l3pt[512] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));

#endif
