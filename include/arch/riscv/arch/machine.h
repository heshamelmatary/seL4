#ifndef __ARCH_MACHINE_H
#define __ARCH_MACHINE_H

#ifndef __ASSEMBLER__
#include <arch/types.h>
#include <arch/object/structures.h>
#include <arch/machine/hardware.h>
#include <arch/encoding.h>
#include <arch/machine/cpu_registers.h>
#include <arch/model/statedata.h>

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

