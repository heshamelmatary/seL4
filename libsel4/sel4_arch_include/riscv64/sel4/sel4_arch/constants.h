/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#ifndef __LIBSEL4_SEL4_ARCH_CONSTANTS_H
#define __LIBSEL4_SEL4_ARCH_CONSTANTS_H

#ifdef HAVE_AUTOCONF
#include <autoconf.h>
#endif

#define seL4_WordBits           64
#define seL4_SlotBits           5
#define seL4_NotificationBits   5
#define seL4_ASIDPoolIndexBits  10
#define seL4_IPCBufferSizeBits  10
#define seL4_MaxUntypedBits     47

/* Sv39/Sv48 pages/ptes sizes */
#define seL4_PageTableEntryBits 3
#define seL4_PageTableIndexBits 9

#define RISCV_Mega_PageBits    21
#define RISCV_Giga_PageBits    30

#if CONFIG_PT_LEVELS == 4
#define RISCV_Tera_PageBits    39
#endif

#define seL4_HugePageBits       RISCV_Giga_PageBits
#define seL4_TeraPageBits       RISCV_Tera_PageBits

/* Shared definitions */
#define RISCV_4K_PageBits      12

#define seL4_TCBBits            11
#define seL4_EndpointBits       4
#define seL4_PageTableBits      12
#define seL4_ASIDPoolBits       12
/* Untyped size limits */
#define seL4_MinUntypedBits 4

/* Map RISC-V-related page sizes to seL4 */
#define seL4_PageBits        RISCV_4K_PageBits
#define seL4_LargePageBits   RISCV_Mega_PageBits

enum {
    seL4_VMFault_IP,
    seL4_VMFault_Addr,
    seL4_VMFault_FSR,
    seL4_VMFault_Length,
} seL4_VMFault_Msg;

enum {
    seL4_UnknownSyscall_FaultIP,
    seL4_UnknownSyscall_SP,
    seL4_UnknownSyscall_RA,
    seL4_UnknownSyscall_A0,
    seL4_UnknownSyscall_A1,
    seL4_UnknownSyscall_A2,
    seL4_UnknownSyscall_A3,
    seL4_UnknownSyscall_A4,
    seL4_UnknownSyscall_A5,
    seL4_UnknownSyscall_A6,
    seL4_UnknownSyscall_Syscall,
    seL4_UnknownSyscall_Length,
} seL4_UnknownSyscall_Msg;

enum {
    seL4_UserException_FaultIP,
    seL4_UserException_SP,
    seL4_UserException_FLAGS,
    seL4_UserException_Number,
    seL4_UserException_Code,
    seL4_UserException_Length,
} seL4_UserException_Msg;

#endif
