DIRECTORIES += src/arch/riscv/

include ${SOURCE_ROOT}/src/arch/riscv/object/Makefile
include ${SOURCE_ROOT}/src/arch/riscv/api/Makefile
include ${SOURCE_ROOT}/src/arch/riscv/kernel/Makefile
include ${SOURCE_ROOT}/src/arch/riscv/model/Makefile
include ${SOURCE_ROOT}/src/arch/riscv/machine/Makefile

ARCH_ASM_SOURCES += head.S traps.S
ARCH_C_SOURCES += c_traps.c idle.c
