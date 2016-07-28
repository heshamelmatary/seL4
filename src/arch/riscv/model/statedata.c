/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <util.h>
#include <api/types.h>
#include <arch/types.h>
#include <arch/model/statedata.h>
#include <arch/object/structures.h>
#include <arch/linker.h>
#include <plat/machine/hardware.h>

/* The global frame, mapped in all address spaces */
word_t riscvKSGlobalsFrame[BIT(PAGE_BITS) / sizeof(word_t)] ALIGN_BSS(BIT(PAGE_BITS));

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[BIT(asidHighBits)];

char riscv_kernel_stack[BIT(RISCV_4K_PageBits)] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
pde_t l1pt[BIT(RISCV_4K_PageBits) / sizeof(pde_t)] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
pte_t l2pt[BIT(RISCV_4K_PageBits) / sizeof(pde_t)] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
pte_t l3pt[BIT(RISCV_4K_PageBits) / sizeof(pde_t)] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
