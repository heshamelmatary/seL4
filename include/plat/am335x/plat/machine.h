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

#define N_INTERRUPTS 128

enum IRQConstants {
    DMTIMER0_IRQ = 66,
    maxIRQ = 127
} platform_interrupt_t;

#define IRQ_CNODE_BITS 12

#define KERNEL_TIMER_IRQ    DMTIMER0_IRQ

enum irqNumbers {
    irqInvalid = 255
};

typedef uint8_t interrupt_t;
typedef uint8_t irq_t;

#endif
/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

