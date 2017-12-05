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

#ifndef __ARCH_MACHINE_HARDWARE_H
#define __ARCH_MACHINE_HARDWARE_H

#define PAGE_BITS 12

/* Kernel PPTR window range is currently (0xFFFFFFFF80000000 - 0xFFFFFFFF90000000) */

/* MMU RISC-V/SV39 related definitions. See RISC-V manual priv-1.10 */
#define SV39_LVL1_PT_BITS  9
#define SV39_LVL2_PT_BITS  9
#define SV39_LVL3_PT_BITS  9

#define SV39_GET_LVL1_PT_INDEX(addr) (((addr) >> 30) & MASK(SV39_LVL1_PT_BITS))
#define SV39_GET_LVL2_PT_INDEX(addr) (((addr) >> 21) & MASK(SV39_LVL2_PT_BITS))
#define SV39_GET_LVL3_PT_INDEX(addr) (((addr) >> 12) & MASK(SV39_LVL2_PT_BITS))

#define L1_CACHE_LINE_SIZE 64

#ifndef __ASSEMBLER__

#include <arch/types.h>
#include <config.h>
#include <arch/types.h>
#include <arch/linker.h>

/*
 * These values are defined in RISC-V priv-1.10 manual, they represent the
 * exception codes saved in scause register (by the hardware) on traps.
 */
enum vm_fault_type {
    RISCVInstructionAccessFault = 1,
    RISCVLoadAccessFault = 5,
    RISCVStoreAccessFault = 7,
    RISCVInstructionPageFault = 12,
    RISCVLoadPageFault = 13,
    RISCVStorePageFault = 15
};
typedef uint32_t vm_fault_type_t;

enum vm_page_size {
    RISCV_4K_Page,
    RISCV_2M_Page,
    RISCV_1G_Page
};

typedef uint32_t vm_page_size_t;

enum PageSizeConstants {
    RISCV_4K_PageBits  = 12,
    RISCV_2M_PageBits  = 21,
    RISCV_1G_PageBits  = 30
};

static inline unsigned int CONST
pageBitsForSize(vm_page_size_t pagesize)
{
    switch (pagesize) {
    case RISCV_4K_Page:
        return RISCV_4K_PageBits;

    case RISCV_2M_Page:
        return RISCV_2M_PageBits;

    case RISCV_1G_Page:
        return RISCV_1G_PageBits;

    default:
        fail("Invalid page size");
    }
}

#endif /* __ASSEMBLER__ */

#endif /* !__ARCH_MACHINE_HARDWARE_H */
