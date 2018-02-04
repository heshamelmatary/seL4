/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <types.h>
#include <machine/io.h>
#include <kernel/vspace.h>
#include <arch/machine.h>
#include <arch/kernel/vspace.h>
#include <plat/machine.h>
#include <linker.h>
#include <plat/machine/devices.h>
#include <plat/machine/hardware.h>
#include <arch/sbi.h>

/* Available physical memory regions on platform (RAM minus kernel image). */
/* NOTE: Regions are not allowed to be adjacent! */

const p_region_t BOOT_RODATA avail_p_regs[] = {
    { .start = 0x00000000C0000000, .end = 0x0000000ffffffff }
};

BOOT_CODE int get_num_avail_p_regs(void)
{
    return sizeof(avail_p_regs) / sizeof(p_region_t);
}

BOOT_CODE p_region_t get_avail_p_reg(unsigned int i)
{
    return avail_p_regs[i];
}


BOOT_CODE int get_num_dev_p_regs(void)
{
    return 0;
}

BOOT_CODE p_region_t get_dev_p_reg(unsigned int i)
{
}

/**
   DONT_TRANSLATE
 */
interrupt_t
getActiveIRQ(void)
{

    uint64_t temp = 0;
    asm volatile ("csrr %0, scause":"=r" (temp)::);

    if (!(temp & 0x8000000000000000UL)) {
        return irqInvalid;
    }

    return (temp & 0xf);
}

/* Check for pending IRQ */
bool_t isIRQPending(void)
{
    return (getActiveIRQ() != irqInvalid);
    /* FIXME: IRQ Pending not implemented now */
}

/* Enable or disable irq according to the 'disable' flag. */
/**
   DONT_TRANSLATE
*/
void
maskInterrupt(bool_t disable, interrupt_t irq)
{
    if (disable) {
        if (irq > 1) {
            clear_csr(sie, BIT(irq));
        }
    } else {
        /* FIXME: currently only account for user/supervisor and timer interrupts */
        if (irq == 4 /* user timer */ || irq == 5 /* supervisor timer */) {
            set_csr(sie, BIT(irq));
        } else {
            /* TODO: account for external and PLIC interrupts */
        }
    }
}

/* Determine if the given IRQ should be reserved by the kernel. */
bool_t CONST
isReservedIRQ(irq_t irq)
{
    printf("isReservedIRQ \n");
    return false;
}

/* Handle a platform-reserved IRQ. */
void
handleReservedIRQ(irq_t irq)
{
    printf("handleReservedIRQ \n");
}

void
ackInterrupt(irq_t irq)
{
    clear_csr(sip, BIT(irq));
    //set_csr(scause, 0);

    if (irq == 1) {
        sbi_clear_ipi();
    }
}

static unsigned long timebase;

static inline uint64_t get_cycles(void)
{
    uint64_t n;
    __asm__ __volatile__ (
        "rdtime %0"
        : "=r" (n));
    return n;
}

static inline int read_current_timer(unsigned long *timer_val)
{
    *timer_val = get_cycles();
    return 0;
}

void
resetTimer(void)
{
    sbi_set_timer(get_cycles() + (0xfffff));
}

/**
   DONT_TRANSLATE
 */
BOOT_CODE void
initTimer(void)
{
    sbi_set_timer(get_cycles() + 0xfffff);
}

static void invalidateL2(void)
{
}

static void finaliseL2Op(void)
{
}

void plat_cleanL2Range(paddr_t start, paddr_t end)
{
}
void plat_invalidateL2Range(paddr_t start, paddr_t end)
{
}

void plat_cleanInvalidateL2Range(paddr_t start, paddr_t end)
{
}

/**
   DONT_TRANSLATE
 */
BOOT_CODE void
initL2Cache(void)
{
}

/**
   DONT_TRANSLATE
 */
BOOT_CODE void
initIRQController(void)
{
    /* Do nothing */
}

void
handleSpuriousIRQ(void)
{
    /* Do nothing */
    printf("Superior IRQ!! \n");
}
