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

#define L1_CACHE_LINE_SIZE 64

#if __riscv_xlen == 32
#define LW lw
#define SW sw
#else /* __riscv_xlen == 64 */
#define LW ld
#define SW sd
#endif

#define PAGE_BITS 12

/* MMU RISC-V related definitions. See RISC-V manual priv-1.10 */

/* Extract the n-level PT index from a virtual address. This works for any
 * configured RISC-V system with CONFIG_PT_LEVEL (which can be 2 on Sv32,
 * 3 on Sv38, or 4 on Sv48)
 */
#define RISCV_GET_PT_INDEX(addr, n)  (((addr) >> (((PT_INDEX_BITS) * ((CONFIG_PT_LEVELS) - (n))) + 12)) & MASK(PT_INDEX_BITS))
#define RISCV_GET_LVL_PGSIZE_BITS(n) (((PT_INDEX_BITS) * (CONFIG_PT_LEVELS - (n))) + 12)
#define RISCV_GET_LVL_PGSIZE(n)      BIT(RISCV_GET_LVL_PGSIZE_BITS((n)))

#ifndef __ASSEMBLER__

#include <arch/types.h>
#include <config.h>
#include <arch/types.h>
#include <linker.h>

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
    RISCV_Mega_Page,
#if CONFIG_PT_LEVELS > 2
    RISCV_Giga_Page,
#endif
#if CONFIG_PT_LEVELS > 3
    RISCV_Tera_Page
#endif
};

typedef uint32_t vm_page_size_t;

#if CONFIG_PT_LEVELS == 2
enum PageSizeConstants {
    RISCV_4K_PageBits  = 12,
    RISCV_Mega_PageBits  = 22,
};
#endif

#if CONFIG_PT_LEVELS > 2
enum PageSizeConstants {
    RISCV_4K_PageBits  = 12,
    RISCV_Mega_PageBits  = 21,
    RISCV_Giga_PageBits  = 30,
#if CONFIG_PT_LEVELS == 4
    RISCV_Tera_PageBits = 39
#endif
};
#endif

static inline unsigned int CONST
pageBitsForSize(vm_page_size_t pagesize)
{
    switch (pagesize) {
    case RISCV_4K_Page:
        return RISCV_4K_PageBits;

    case RISCV_Mega_Page:
        return RISCV_Mega_PageBits;

#if CONFIG_PT_LEVELS > 2
    case RISCV_Giga_Page:
        return RISCV_Giga_PageBits;
#endif

#if CONFIG_PT_LEVELS == 4
    case RISCV_Tera_Page:
        return RISCV_Tera_PageBits;
#endif

    default:
        fail("Invalid page size");
    }
}

/* For optimisation/simplicity, this function assumes size types start from 0 linearly, otherwise it won't work */
static inline uint32_t CONST
RISCVpageAtPTLevel(vm_page_size_t pagesize)
{
    return (CONFIG_PT_LEVELS - pagesize);
}

#endif /* __ASSEMBLER__ */

#endif /* !__ARCH_MACHINE_HARDWARE_H */
