/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <stdint.h>
#include <util.h>
#include <machine/io.h>
#include <plat/machine/devices.h>
#include <arch/machine.h>
#include <arch/sbi.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

static inline print_format_cause(int cause_num)
{
    switch (cause_num) {
    case 0:
        printf("Instruction address misaligned\n");
        return;
    case 1:
        printf("Instruction access fault\n");
        return;
    case 2:
        printf("Illegal instruction\n");
        return;
    case 3:
        printf("Breakpoint\n");
        return;
    case 5:
        printf("Load access fault\n");
        return;
    case 6:
        printf("AMO address misaligned\n");
        return;
    case 7:
        printf("Store/AMO access fault\n");
        return;
    case 8:
        printf("Environment call\n");
        return;
    default:
        printf("Reserved cause %d\n", cause_num);
        return;
    }
}

void handle_exception(void)
{
    register uint64_t thread_context asm("t0");
    register uint64_t scause asm("a0");
    register uint64_t sepc asm("a1");
    register uint64_t sbadaddr asm("a2");
    register uint64_t sstatus asm("a3");
    register uint64_t ra asm("ra");

    asm volatile ("csrr %0, scause\n"
                  "csrr %1, sepc\n"
                  "csrr %2, sbadaddr\n"
                  "csrr %3, sstatus\n"
                  : "=r" (scause),
                  "=r" (sepc),
                  "=r" (sbadaddr),
                  "=r" (sstatus)
                  ::);

    /* handleVMFaultEvent
     * 1 -> Instruction access fault
     * 5 -> Load access fault
     * 7 -> Store/AMO access fault
     * 12 -> Instruction page fault
     * 13 -> Load page fault
     * 15 -> Store/AMO page fault
     * */
    if (scause == 5 || scause == 7 || scause == 1 || scause == 12 || scause == 13 || scause == 15) {
        handleVMFaultEvent(scause);
        return;
    }

    /* Illegal/Undef instruction */
    if (scause == 2) {
        handleUserLevelFault(0, 0);
        return;
    }

    print_format_cause(scause);
    printf("sepc = %p\n", sepc);
    printf("sbadaddr = %p\n", sbadaddr);
    printf("sstatus = %p\n", sstatus);
    printf(KRED "Halt!\n");

    printf(KBLU"Register Context Dump \n");
    for (int i = 0; i < 32; i++) {
        printf("x%d = %p\n", i, *(((uint64_t *) thread_context) + i));
    }

    halt();
}

#ifdef CONFIG_PRINTING
void
qemu_uart_putchar(char c)
{
}

unsigned char getDebugChar(void)
{
}

VISIBLE void halt(void)
{
    sbi_shutdown();
}

void putDebugChar(unsigned char c)
{
    sbi_console_putchar(c);
}

void
putConsoleChar(unsigned char c)
{
    putDebugChar(c);
}
#endif
