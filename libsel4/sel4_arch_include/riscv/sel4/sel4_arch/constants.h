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

#include <autoconf.h>

#ifdef CONFIG_ARCH_RISCV_RV32
#define seL4_WordBits           32
#define seL4_SlotBits           4
#define seL4_NotificationBits   4
#define seL4_ASIDPoolIndexBits  9
#define seL4_IPCBufferSizeBits  9
#define seL4_MaxUntypedBits     29

/* Sv32 pages/ptes sizes */
#define seL4_PageTableEntryBits 2
#define seL4_PageTableIndexBits 10
#define seL4_LargePageBits      22

#elif defined(CONFIG_ARCH_RISCV_RV64)

#define seL4_WordBits           64
#define seL4_SlotBits           5
#define seL4_NotificationBits   5
#define seL4_ASIDPoolIndexBits  10
#define seL4_IPCBufferSizeBits  10
#define seL4_MaxUntypedBits     47

/* Sv39/Sv48 pages/ptes sizes */
#define seL4_PageTableEntryBits 3
#define seL4_PageTableIndexBits 9
#define seL4_LargePageBits      21
#define seL4_HugePageBits       30

/* Only for Sv48 */
#if CONFIG_PT_LEVELS == 4
#define seL4_TeraPageBits       39
#endif /* CONFIG_PT_LEVELS == 4 */

#else /* CONFIG_ARCH_RISCV_* */
#error "Wrong RISC-V mode"
#endif /* CONFIG_ARCH_RISCV_RV32 */

/* Shared definitions */
#define seL4_PageBits           12
#define seL4_TCBBits            11
#define seL4_EndpointBits       4
#define seL4_PageTableBits      12
#define seL4_ASIDPoolBits       12
/* Untyped size limits */
#define seL4_MinUntypedBits 4

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