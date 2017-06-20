/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

#ifndef __ARCH_MACHINE_H
#define __ARCH_MACHINE_H

#ifndef __ASSEMBLER__
#include <arch/types.h>
#include <arch/object/structures.h>
#include <arch/machine/hardware.h>
#include <arch/encoding.h>
#include <arch/machine/cpu_registers.h>
#include <arch/model/statedata.h>
#include <arch/model/smp.h>
#include <arch/sbi.h>

#if CONFIG_MAX_NUM_NODES > 1
/* Use the first two SGI (Software Generated Interrupt) IDs
 * for seL4 IPI implementation. SGIs are per-core banked.
 */
#define irq_remote_call_ipi        0
#define irq_reschedule_ipi         1
#define int_remote_call_ipi       irq_remote_call_ipi
#define int_reschedule_ipi        irq_reschedule_ipi

#define IPI_MEM_BARRIER asm volatile("sfence.vma");

void ipi_send_target(irq_t irq, word_t cpuTargetList)
{
    sbi_send_ipi(&cpuTargetList);
}

#endif /* CONFIG_MAX_NUM_NODES > 1 */

word_t PURE getRestartPC(tcb_t *thread);
void setNextPC(tcb_t *thread, word_t v);

/* Cleaning memory before user-level access */
static inline void clearMemory(void* ptr, unsigned int bits)
{
    memzero(ptr, BIT(bits));
}

static inline void setCurrentPD(paddr_t addr, asid_t asid)
{
    write_csr(sptbr, SPTBR_MODE_SV39 << CTZL(SPTBR64_MODE)   |
              ((uint64_t) asid) << CTZL(SPTBR64_ASID) |
              (addr >> RISCV_4K_PageBits));
}


#if CONFIG_MAX_NUM_NODES > 1
static inline void arch_pause(void)
{
}
#endif

static inline void Arch_finaliseInterrupt(void)
{
}

#endif // __ASSEMBLER__
#endif

