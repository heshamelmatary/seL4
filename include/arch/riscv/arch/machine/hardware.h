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

#ifndef __ASSEMBLER__

#include <config.h>
#include <linker.h>

#include <arch/types.h>
#include <mode/api/constants.h>

#define PAGE_BITS RISCV_4K_PageBits

/* MMU RISC-V related definitions. See RISC-V manual priv-1.10 */

/* Extract the n-level PT index from a virtual address. This works for any
 * configured RISC-V system with CONFIG_PT_LEVEL (which can be 2 on Sv32,
 * 3 on Sv38, or 4 on Sv48)
 */
#define RISCV_GET_PT_INDEX(addr, n)  (((addr) >> (((PT_INDEX_BITS) * ((CONFIG_PT_LEVELS) - (n))) + RISCV_4K_PageBits)) & MASK(PT_INDEX_BITS))
#define RISCV_GET_LVL_PGSIZE_BITS(n) (((PT_INDEX_BITS) * (CONFIG_PT_LEVELS - (n))) + RISCV_4K_PageBits)
#define RISCV_GET_LVL_PGSIZE(n)      BIT(RISCV_GET_LVL_PGSIZE_BITS((n)))
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

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#else /* __riscv_xlen == 64 */
#define LOAD  ld
#define STORE sd
#endif

#if __riscv_flen == 32
#define FLOAD  flw
#define FSTORE fsw
#elif __riscv_flen == 64
#define FLOAD  fld
#define FSTORE fsd
#endif /* CONFIG_HAVE_FPU */

#define _STRINGIFY(a) #a
#define STRINGIFY(a) _STRINGIFY(a)

#define LOAD_S STRINGIFY(LOAD)
#define STORE_S STRINGIFY(STORE)

#define FLOAD_S STRINGIFY(FLOAD)
#define FSTORE_S STRINGIFY(FSTORE)

#endif /* !__ARCH_MACHINE_HARDWARE_H */
