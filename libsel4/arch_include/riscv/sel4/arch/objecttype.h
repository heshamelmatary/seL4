/*
 * Copyright 2016, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#ifndef __ARCH_OBJECTTYPE_H
#define __ARCH_OBJECTTYPE_H

typedef enum _object {
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

/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

