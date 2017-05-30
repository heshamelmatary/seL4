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

#define PPTR_VECTOR_TABLE 0xFFFFFFFF80000000ULL
#define PPTR_GLOBALS_PAGE 0xFFFFFFFF8FE00000ULL

/* The stack is the very last page of virtual memory. */
/* TODO: The following stack definition is to be removed with multicore support */
#define PPTR_KERNEL_STACK 0xFFFFFFFF8FFF1000
#define PPTR_KERNEL_STACK_TOP PPTR_KERNEL_STACK + 0x1000 - 16

/* MMU RISC-V/SV39 related definitions. See RISC-V manual priv-1.10 */
#define SV39_VIRT_TO_VPN2(addr) ((addr) >> 30)
#define SV39_VIRT_TO_VPN1(addr) ((addr) >> 21)
#define SV39_VIRT_TO_VPN0(addr) ((addr) >> 12)

#define SV39_LVL1_PT_BITS  9
#define SV39_LVL2_PT_BITS  9
#define SV39_LVL3_PT_BITS  9

#define SV39_GET_LVL1_PT_INDEX(addr) (((addr) >> 30) & MASK(SV39_LVL1_PT_BITS))
#define SV39_GET_LVL2_PT_INDEX(addr) (((addr) >> 21) & MASK(SV39_LVL2_PT_BITS))
#define SV39_GET_LVL3_PT_INDEX(addr) (((addr) >> 12) & MASK(SV39_LVL2_PT_BITS))

#ifndef __ASSEMBLER__

#include <arch/types.h>
#include <config.h>
#include <arch/types.h>
#include <arch/linker.h>

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

enum pte_type {
    RISCV_PTE_TYPE_TABLE = 0,
    RISCV_PTE_TYPE_TABLE_GLOBAL = 0x10,
    RISCV_PTE_TYPE_UR_SR = 0x9,
    RISCV_PTE_TYPE_URW_SRW = 0xB,
    RISCV_PTE_TYPE_URX_SRX = 0xD,
    RISCV_PTE_TYPE_URWX_SRWX = 0xF,
    RISCV_PTE_TYPE_SR = 0x1,
    RISCV_PTE_TYPE_SRW = 0x3,
    RISCV_PTE_TYPE_SRX = 0x5,
    RISCV_PTE_TYPE_SRWX = 0x7,
    RISCV_PTE_TYPE_SR_GLOBAL = 0x11,
    RISCV_PTE_TYPE_SRW_GLOBAL = 0x13,
    RISCV_PTE_TYPE_SRX_GLOBAL = 0x15,
    RISCV_PTE_TYPE_SRWX_GLOBAL = 0x17
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
