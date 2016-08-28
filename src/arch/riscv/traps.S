#define __ASSEMBLER__ 1
#include <config.h>
#include <machine/assembler.h>
#include <arch/machine/hardware.h>

.section .boot.text, "ax"
.global _start
.global trap_entry
.extern handleUnknownSyscall
.extern global_start

#define LREG ld
#define SREG sd
#define REGBYTES 8

_start:
1:
  auipc gp, %pcrel_hi(_gp)
  addi gp, gp, %pcrel_lo(1b)
  la s0, global_start
  jr s0

.section .vector, "ax"
.balign 4096

#include <arch/api/syscall.h>
#include <arch/machine/registerset.h>

trap_entry:

  csrrw t0, sscratch, t0

  SREG ra, 1*REGBYTES(t0)
  SREG sp, 2*REGBYTES(t0)
  SREG gp, 3*REGBYTES(t0)
  SREG tp, 4*REGBYTES(t0)
  SREG t0, 5*REGBYTES(t0)
  SREG t1, 6*REGBYTES(t0)
  SREG t2, 7*REGBYTES(t0)
  SREG s0, 8*REGBYTES(t0)
  SREG s1, 9*REGBYTES(t0)
  SREG a0, 10*REGBYTES(t0)
  SREG a1, 11*REGBYTES(t0)
  SREG a2, 12*REGBYTES(t0)
  SREG a3, 13*REGBYTES(t0)
  SREG a4, 14*REGBYTES(t0)
  SREG a5, 15*REGBYTES(t0)
  SREG a6, 16*REGBYTES(t0)
  SREG a7, 17*REGBYTES(t0)
  SREG s2, 18*REGBYTES(t0)
  SREG s3, 19*REGBYTES(t0)
  SREG s4, 20*REGBYTES(t0)
  SREG s5, 21*REGBYTES(t0)
  SREG s6, 22*REGBYTES(t0)
  SREG s7, 23*REGBYTES(t0)
  SREG s8, 24*REGBYTES(t0)
  SREG s9, 25*REGBYTES(t0)
  SREG s10, 26*REGBYTES(t0)
  SREG s11, 27*REGBYTES(t0)
  SREG t3, 28*REGBYTES(t0)
  SREG t4, 29*REGBYTES(t0)
  SREG t5, 30*REGBYTES(t0)
  SREG t6, 31*REGBYTES(t0)

  csrr x1, sstatus
  SREG x1, 32*REGBYTES(t0)

  li sp, PPTR_KERNEL_STACK_TOP

  /* Check if it's an interrupt */
  csrr s0, scause
  csrr s3, scause

  srli s0, s0, 63
  li   s1, 0x1
  beq  s0, s1, handle_irq

  andi s3, s3, 0xf
  li   s4, 8
  blt  s3, s4, _error
  j _syscall

_error:
  la x1, halt
  jr x1
    /* Set the return address to sepc + 4 in the case of a systemcall */
_syscall:
  csrr s2,  sepc
  addi s2, s2, 4
  csrw sepc, s2

  SREG s2, 33*REGBYTES(t0)

  /* Move systemcall number to a0 */
  mv a0, a7

  /* Check that syscall number is in range */
  li  s0, SYSCALL_MIN
  blt a0, s0, unknownsyscall
  nop
  la  s0, handleSyscall
  jalr s0
  /* SREG s2, 1*REGBYTES(t0) */
  nop
  j   ret_user

unknownsyscall:
  la s0, handleUnknownSyscall
  jalr s0
  j   ret_user

handle_irq:
  la   s0, handleInterruptEntry
  jalr s0

ret_user:

  la t0, ksCurThread
  ld t0, 0(t0)

  LREG x1, 32*REGBYTES(t0)
  csrw sstatus, x1

  LREG ra, 1*REGBYTES(t0)
  LREG sp, 2*REGBYTES(t0)
  LREG gp, 3*REGBYTES(t0)

  /* skip tp */
  /* skip x5/t0 */
  LREG t2, 7*REGBYTES(t0)
  LREG s0, 8*REGBYTES(t0)
  LREG s1, 9*REGBYTES(t0)
  LREG a0, 10*REGBYTES(t0)
  LREG a1, 11*REGBYTES(t0)
  LREG a2, 12*REGBYTES(t0)
  LREG a3, 13*REGBYTES(t0)
  LREG a4, 14*REGBYTES(t0)
  LREG a5, 15*REGBYTES(t0)
  LREG a6, 16*REGBYTES(t0)
  LREG a7, 17*REGBYTES(t0)
  LREG s2, 18*REGBYTES(t0)
  LREG s3, 19*REGBYTES(t0)
  LREG s4, 20*REGBYTES(t0)
  LREG s5, 21*REGBYTES(t0)
  LREG s6, 22*REGBYTES(t0)
  LREG s7, 23*REGBYTES(t0)
  LREG s8, 24*REGBYTES(t0)
  LREG s9, 25*REGBYTES(t0)
  LREG s10, 26*REGBYTES(t0)
  LREG s11, 27*REGBYTES(t0)
  LREG t3, 28*REGBYTES(t0)
  LREG t4, 29*REGBYTES(t0)
  LREG t5, 30*REGBYTES(t0)
  LREG t6, 31*REGBYTES(t0)

  /* Get next restored tp */
  LREG t1, 4*REGBYTES(t0)

  /* get restored tp */
  add tp, t1, x0

  /* get sepc */
  LREG t1, 33*REGBYTES(t0)
  csrw sepc, t1

  LREG t1, 6*REGBYTES(t0)
  csrrw t0, sscratch, t0
  eret
