/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#include <config.h>
#include <machine/assembler.h>
#include <arch/machine/hardware.h>
#include <arch/api/syscall.h>
#include <arch/machine/registerset.h>
#include <util.h>

#define REGBYTES (CONFIG_WORD_SIZE / 8)

.global trap_entry
.extern c_handle_syscall
.extern c_handle_interrupt
.extern c_handle_exception
.extern restore_user_context

.section .vector, "ax"
.balign 4096

trap_entry:

  csrrw t0, sscratch, t0

  sd ra, (0*REGBYTES)(t0)
  sd sp, (1*REGBYTES)(t0)
  sd gp, (2*REGBYTES)(t0)
  sd tp, (3*REGBYTES)(t0)
  sd t1, (5*REGBYTES)(t0)
  sd t2, (6*REGBYTES)(t0)
  sd s0, (7*REGBYTES)(t0)
  sd s1, (8*REGBYTES)(t0)
  sd a0, (9*REGBYTES)(t0)
  sd a1, (10*REGBYTES)(t0)
  sd a2, (11*REGBYTES)(t0)
  sd a3, (12*REGBYTES)(t0)
  sd a4, (13*REGBYTES)(t0)
  sd a5, (14*REGBYTES)(t0)
  sd a6, (15*REGBYTES)(t0)
  sd a7, (16*REGBYTES)(t0)
  sd s2, (17*REGBYTES)(t0)
  sd s3, (18*REGBYTES)(t0)
  sd s4, (19*REGBYTES)(t0)
  sd s5, (20*REGBYTES)(t0)
  sd s6, (21*REGBYTES)(t0)
  sd s7, (22*REGBYTES)(t0)
  sd s8, (23*REGBYTES)(t0)
  sd s9, (24*REGBYTES)(t0)
  sd s10, (25*REGBYTES)(t0)
  sd s11, (26*REGBYTES)(t0)
  sd t3, (27*REGBYTES)(t0)
  sd t4, (28*REGBYTES)(t0)
  sd t5, (29*REGBYTES)(t0)
  sd t6, (30*REGBYTES)(t0)

  /* save t0 value */
  csrr  x1, sscratch
  sd    x1, (4*REGBYTES)(t0)

  csrr x1, sstatus
  sd x1, (32*REGBYTES)(t0)

  /* disable all "supervisor" interrupts (i.e. timer and IPI) */
  csrw sstatus, x0

  csrr s0, scause
  sd s0, (31*REGBYTES)(t0)

  /* Save exception PC */
  csrr x1,  sepc
  sd   x1, (33*REGBYTES)(t0)

  la gp, __global_pointer$

  /* Load kernel's stack address */
  la sp, (kernel_stack_alloc + BIT(CONFIG_KERNEL_STACK_BITS))

  /* Check if it's an interrupt */
  srli s2, s0, 63
  li   s1, 0x1
  beq  s2, s1, interrupt

  andi s0, s0, 0xf /* priv 1.10 defines up to 15 exceptions/interrupts */
  li   s4, 8       /* priv 1.10 has value 8 for ecall exception */
  bne  s0, s4, exception

syscall:
  /* Set the return address to sepc + 4 in the case of a system/environment call */
  addi x1, x1, 4
  /* Save NEXTPC */
  sd   x1, (35*REGBYTES)(t0)

  j c_handle_syscall

/* Not an interrupt or a syscall */
exception:
  /* Save NEXTPC */
  sd   x1, (35*REGBYTES)(t0)
  j c_handle_exception

interrupt:
  /* Save NEXTPC */
  sd   x1, (35*REGBYTES)(t0)
  j c_handle_interrupt
