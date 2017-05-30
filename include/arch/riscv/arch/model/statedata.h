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

/* RISC-V has 3-level page tables, each of which is 4KiB, PTEs are of word_t size */
extern pde_t l1pt[BIT(RISCV_4K_PageBits) / sizeof(word_t)] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
extern pte_t l2pt[BIT(RISCV_4K_PageBits) / sizeof(word_t)] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));
extern pte_t l3pt[BIT(RISCV_4K_PageBits) / sizeof(word_t)] __attribute__ ((aligned(BIT(RISCV_4K_PageBits))));

#endif
/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

