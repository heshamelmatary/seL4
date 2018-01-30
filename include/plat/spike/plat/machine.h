/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __PLAT_MACHINE_H
#define __PLAT_MACHINE_H


#define N_INTERRUPTS 6

#define wordRadix 6
#define wordBits (1 << wordRadix)

#ifndef __ASSEMBLER__
enum IRQConstants {
    INTERRUPT_SW = 0,
    INTERRUPT_TIMER = 5,
    /* We don't need other IRQs for now at least */
    maxIRQ = 5
} platform_interrupt_t;

#define IRQ_CNODE_BITS 12
#define KERNEL_TIMER_IRQ INTERRUPT_TIMER

enum irqNumbers {
    irqInvalid = 6
};

typedef uint32_t interrupt_t;
typedef uint32_t irq_t;

#endif /* !__ASSEMBLER__ */
#endif
