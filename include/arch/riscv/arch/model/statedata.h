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


NODE_STATE_BEGIN(archNodeState)
/* TODO: add RISCV-dependent fields here */
/* Bitmask of all cores should receive the reschedule IPI */
NODE_STATE_DECLARE(word_t, ipiReschedulePending);
NODE_STATE_END(archNodeState);

extern asid_pool_t *riscvKSASIDTable[BIT(asidHighBits)];

/* RISC-V has 3-level page tables, each of which is 4KiB, PTEs are of word_t size */
extern pte_t l1pt[BIT(seL4_PageDirBits) / sizeof(word_t)] ALIGN(BIT(seL4_PageDirBits));
extern pte_t l2pt[BIT(seL4_PageDirBits) / sizeof(word_t)] ALIGN(BIT(seL4_PageDirBits));
extern pte_t l3pt[BIT(seL4_PageDirBits) / sizeof(word_t)] ALIGN(BIT(seL4_PageDirBits));
#ifdef CONFIG_RISCV_SV48
extern pte_t l4pt[BIT(seL4_PageDirBits) / sizeof(word_t)] ALIGN(BIT(seL4_PageTableBits));
#endif /* CONFIG_RISCV_SV48 */

#endif
