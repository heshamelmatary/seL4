/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#ifndef __LIBSEL4_SEL4_ARCH_EXCEPTION_IPC
#define __LIBSEL4_SEL4_ARCH_EXCEPTION_IPC

/**
 * NOT A STANDALONE INCLUDE
 */

/* Unknown syscall exception message. */
#define SEL4_EXCEPT_IPC_LABEL      3

/* User exception (such as divide by zero) message. */
#define SEL4_USER_EXCEPTION_LABEL  4
#define SEL4_USER_EXCEPTION_LENGTH 5

typedef enum {
    EXCEPT_IPC_USER_MR_FAULT_IP,
    EXCEPT_IPC_USER_MR_SP,
    EXCEPT_IPC_USER_MR_NUMBER,
    SEL4_USER_EXCEPT_IPC_LENGTH
} seL4_UserExceptIPCRegister;

typedef enum {
    EXCEPT_IPC_SYS_MR_SP,
    EXCEPT_IPC_SYS_MR_RA,
    EXCEPT_IPC_SYS_MR_A0,
    EXCEPT_IPC_SYS_MR_A1,
    EXCEPT_IPC_SYS_MR_A2,
    EXCEPT_IPC_SYS_MR_A3,
    EXCEPT_IPC_SYS_MR_A4,
    EXCEPT_IPC_SYS_MR_A5,
    EXCEPT_IPC_SYS_MR_A6,
    EXCEPT_IPC_SYS_MR_A7,
    EXCEPT_IPC_SYS_MR_SYSCALL,
    SEL4_EXCEPT_IPC_LENGTH,
} seL4_ExceptIPCRegister;

typedef union {
    struct {
        seL4_Word ra;
        seL4_Word sp;
        seL4_Word exception_number;
        seL4_Word exception_code;
    } regs;
    seL4_Word raw[4];
} seL4_UserExceptionIpcRegisters;

typedef union {
    struct {
        seL4_Word ra;
        seL4_Word sp;
        seL4_Word a0;
        seL4_Word a1;
        seL4_Word a2;
        seL4_Word a3;
        seL4_Word a4;
        seL4_Word a5;
        seL4_Word a6;
        seL4_Word a7;
        seL4_Word syscall;
    } regs;
    seL4_Word raw[11];
} seL4_ExceptionIpcRegisters;

static inline seL4_Word seL4_isExceptIPC_Tag(seL4_MessageInfo_t tag)
{
    return seL4_MessageInfo_get_label(tag) == SEL4_EXCEPT_IPC_LABEL;
}

static inline seL4_Word seL4_ExceptIPC_Get(seL4_Word mr)
{
    return seL4_GetMR(mr);
}

static inline void seL4_ExceptIPC_Set(seL4_Word index, seL4_Word val)
{
    seL4_SetMR(index, val);
}

static inline seL4_Word seL4_IsArchSyscallFrom(seL4_MessageInfo_t tag)
{
    return seL4_MessageInfo_get_length(tag) == SEL4_EXCEPT_IPC_LENGTH;
}

static inline seL4_Word seL4_IsArchExceptionFrom(seL4_MessageInfo_t tag)
{
    return seL4_MessageInfo_get_length(tag) == SEL4_USER_EXCEPTION_LENGTH;
}

#endif
