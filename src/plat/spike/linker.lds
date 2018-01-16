/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

OUTPUT_FORMAT("elf64-littleriscv", "elf64-littleriscv", "elf64-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

/* WARNING: constants also defined in plat/machine/hardware.h */

/* #ifndef CONFIG_RISCV_SV48
KERNEL_BASE   = 0xFFFFFFFF80000000;
#else
*/
KERNEL_BASE   = 0xFFFFFF8000000000;
/* #endif */
PHYS_BASE     = 0x0000000080C00000;
KERNEL_OFFSET = KERNEL_BASE - PHYS_BASE;

SECTIONS
{
    . = . + KERNEL_BASE;

    .boot . : AT(ADDR(.boot) - KERNEL_OFFSET)
    {
        *(.boot.text)
        *(.boot.rodata)
        *(.boot.data)
        . = ALIGN(64K);
    }


    ki_boot_end = .;

    .text . : AT(ADDR(.text) - KERNEL_OFFSET)
    {
        /* Sit inside a large frame */
        . = ALIGN(4K);

        /* Hopefully all that fits into 4K! */
        /* Fastpath code */
        *(.vectors)
        *(.vectors.fastpath_call)
        *(.vectors.fastpath_reply_wait)
        *(.vectors.text)
        /* Anything else that should be in the vectors page. */
        *(.vectors.*)


        /* Standard kernel */
        *(.text)
    }

    /* Start of data section */
    _sdata = .;
    .sdata : {
        __global_pointer$ = . + 0x800;
        *(.sdata*)
    }
    .srodata : {
        *(.srodata*)
    }

    .rodata . : AT(ADDR(.rodata) - KERNEL_OFFSET)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data . : AT(ADDR(.data) - KERNEL_OFFSET)
    {
        *(.data)
    }

    .bss . : AT(ADDR(.bss) - KERNEL_OFFSET)
    {
        *(.bss)

        /* 4k breakpoint stack */
        _breakpoint_stack_bottom = .;
        . = . + 4K;
        _breakpoint_stack_top = .;

        /* large data such as the globals frame and global PD */
        *(.bss.aligned)
        . = . + 8;
    }

    . = ALIGN(4K);
    ki_end = .;
}
