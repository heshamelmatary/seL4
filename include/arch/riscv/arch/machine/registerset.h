#ifndef __ARCH_MACHINE_REGISTERSET_H
#define __ARCH_MACHINE_REGISTERSET_H

#include "hardware.h"

#define PT_SP               (2  * 8)
#define PT_A0               (10 * 8)

#define SSTATUS_FS 0x00006000

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <util.h>
#include <arch/types.h>

enum _register {

    ra = 1, LR = 1,

    sp = 2, SP = 2,
    gp = 3,
    tp = 4,

    t0 = 5,
    t1 = 6,
    t2 = 7,
    s0 = 8,
    s1 = 9,

    /* x10-x17 > a0-a7 */
    a0 = 10, capRegister = 10, badgeRegister = 10,
    a1 = 11, msgInfoRegister = 11,
    a2 = 12,
    a3 = 13,
    a4 = 14,
    a5 = 15,
    a6 = 16,
    a7 = 17,

    s2 = 18,
    s3 = 19,
    s4 = 20,
    s5 = 21,
    s6 = 22,
    s7 = 23,
    s8 = 24,
    s9 = 25,
    s10 = 26,
    s11 = 27,

    t3 = 28,
    t4 = 29,
    t5 = 30,
    t6 = 31,

    /* End of GP registers, the following are additional kernel-saved state. */
    SSTATUS,
    SEPC,

    n_contextRegisters
};

typedef uint64_t register_t;

/* FIXME */
enum messageSizes {
    n_msgRegisters = 4,
    n_frameRegisters = 17,
    n_gpRegisters = 0,
    n_exceptionMessage = 3,
    n_syscallMessage = 12
};

extern const register_t msgRegisters[] VISIBLE;
extern const register_t frameRegisters[] VISIBLE;
extern const register_t gpRegisters[] VISIBLE;
extern const register_t exceptionMessage[] VISIBLE;
extern const register_t syscallMessage[] VISIBLE;

struct user_context {
    word_t registers[n_contextRegisters];
};
typedef struct user_context user_context_t;

static inline void Arch_initContext(user_context_t* context)
{
    /* Always enable floating point for new threads */
    context->registers[SSTATUS] = SSTATUS_FS;
}

static inline word_t CONST
sanitiseRegister(register_t reg, word_t v)
{
    /* FIXME */
    return v;
}

#endif /* __ASSEMBLER__ */

#endif /* !__ARCH_MACHINE_REGISTERSET_H */
