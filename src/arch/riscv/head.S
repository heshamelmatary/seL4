#include <arch/machine/registerset.h>

.section .boot.text, "ax"
.global global_start
.extern init_kernel
.extern riscv_kernel_stack

global_start:
  la gp, _gp

  la sp, (riscv_kernel_stack)
  jal init_kernel
  
  /* Load initial thread context */
  la tp, ksCurThread
  ld tp, 0(tp)
  ld sp, PT_SP(tp)
  /* Load a0 (bi_frame) */
  ld a0, PT_A0(tp)
  
  ld sp, 0(tp)
  eret
