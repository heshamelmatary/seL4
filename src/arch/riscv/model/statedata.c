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

#include <util.h>
#include <api/types.h>
#include <arch/types.h>
#include <arch/model/statedata.h>
#include <arch/object/structures.h>
#include <linker.h>
#include <plat/machine/hardware.h>

/* The top level asid mapping table */
asid_pool_t *riscvKSASIDTable[BIT(asidHighBits)];

pte_t l1pt[BIT(seL4_PageDirBits) / sizeof(word_t)] ALIGN(BIT(seL4_PageTableBits));
pte_t l2pt[BIT(seL4_PageDirBits) / sizeof(word_t)] ALIGN(BIT(seL4_PageTableBits));
pte_t l3pt[BIT(seL4_PageDirBits) / sizeof(word_t)] ALIGN(BIT(seL4_PageTableBits));
#ifdef CONFIG_RISCV_SV48
pte_t l4pt[BIT(seL4_PageDirBits) / sizeof(word_t)] ALIGN(BIT(seL4_PageTableBits));
#endif /* CONFIG_RISCV_SV48 */
