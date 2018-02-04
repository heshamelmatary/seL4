/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#ifndef __LIBSEL4_SEL4_ARCH_TYPES_H
#define __LIBSEL4_SEL4_ARCH_TYPES_H

#include <autoconf.h>
#include <sel4/simple_types.h>

#ifdef CONFIG_ARCH_RISCV_RV32
#define seL4_WordBits        32
typedef seL4_Uint32 seL4_Word;
#else
#define seL4_WordBits        64
typedef seL4_Uint64 seL4_Word;
#endif

typedef seL4_Word seL4_NodeId;
typedef seL4_Word seL4_PAddr;
typedef seL4_Word seL4_Domain;

#define seL4_PageBits        12
#define seL4_EndpointBits     4

typedef seL4_Word seL4_CPtr;
typedef seL4_CPtr seL4_RISCV_Page;
typedef seL4_CPtr seL4_RISCV_PageTable;
typedef seL4_CPtr seL4_RISCV_IPI;

/* User context as used by seL4_TCB_ReadRegisters / seL4_TCB_WriteRegisters */

typedef struct seL4_UserContext_ {
    seL4_Word pc;
    seL4_Word ra;
    seL4_Word sp;
    seL4_Word x3;
    seL4_Word x4;
    seL4_Word t0;
    seL4_Word t1;
    seL4_Word t2;
    seL4_Word s0;
    seL4_Word s1;
    seL4_Word a0;
    seL4_Word a1;
    seL4_Word a2;
    seL4_Word a3;
    seL4_Word a4;
    seL4_Word a5;
    seL4_Word a6;
} seL4_UserContext;

typedef enum {
    seL4_RISCV_Default_VMAttributes = 7,

    seL4_RISCV_PTE_TYPE_TABLE = 0,
    seL4_RISCV_PTE_TYPE_TABLE_GLOBAL = 1,
    seL4_RISCV_PTE_TYPE_URX_SR = 2,
    seL4_RISCV_PTE_TYPE_URWX_SRW = 3,
    seL4_RISCV_PTE_TYPE_UR_SR = 4,
    seL4_RISCV_PTE_TYPE_URW_SRW = 5,
    seL4_RISCV_PTE_TYPE_URX_SRX = 6,
    seL4_RISCV_PTE_TYPE_URWX_SRWX = 7,
    seL4_RISCV_PTE_TYPE_SR = 8,
    seL4_RISCV_PTE_TYPE_SRW = 9,
    seL4_RISCV_PTE_TYPE_SRX = 10,
    seL4_RISCV_PTE_TYPE_SRWX = 11,
    seL4_RISCV_PTE_TYPE_SR_GLOBAL = 12,
    seL4_RISCV_PTE_TYPE_SRW_GLOBAL = 13,
    seL4_RISCV_PTE_TYPE_SRX_GLOBAL = 14,
    seL4_RISCV_PTE_TYPE_SRWX_GLOBAL = 15,
    SEL4_FORCE_LONG_ENUM(seL4_RISCV_VMAttributes)
} seL4_RISCV_VMAttributes;

#endif