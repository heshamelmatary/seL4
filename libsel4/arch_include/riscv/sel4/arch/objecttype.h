/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#ifndef __ARCH_OBJECTTYPE_H
#define __ARCH_OBJECTTYPE_H

typedef enum _object {
    /* RISC-V's Sv39 paging mode */
    seL4_RISCV_4K_Page = seL4_NonArchObjectTypeCount,
    seL4_RISCV_2M_Page,
    seL4_RISCV_1G_Page,
    seL4_RISCV_PageTableObject,
    seL4_RISCV_LVL2PageTableObject,
    seL4_RISCV_PageDirectoryObject,
    seL4_ObjectTypeCount
} seL4_ArchObjectType;

typedef seL4_Word object_t;

#endif /* __ARCH_OBJECTTYPE_H */

