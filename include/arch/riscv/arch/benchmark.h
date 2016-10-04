#ifndef ARCH_BENCHMARK_H
#define ARCH_BENCHMARK_H

#include <config.h>
#include <arch/object/structures.h>

#ifdef CONFIG_ENABLE_BENCHMARK
#warning "RISC-V doesn't support timestamp() function yet"

extern uint64_t ksEntry;
extern uint64_t ksExit;
extern uint32_t ksLogIndex;
extern uint32_t *ksLog;

static inline uint64_t
timestamp(void)
{
}

#endif /* CONFIG_ENABLE_BENCHMARK */
#endif /* ARCH_BENCHMARK_H */
