#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the GNU General Public License version 2. Note that NO WARRANTY is provided.
# See "LICENSE_GPLv2.txt" for details.
#
# @TAG(DATA61_GPL)
#

cmake_minimum_required(VERSION 3.7.2)

config_choice(KernelRISCVSel4Arch RISCV_SEL4_ARCH "Architecture mode for building the kernel"
    "riscv;KernelSel4ArchRISCV;ARCH_RISCV;KernelArchRISCV"
)

config_choice(KernelRISCVPlatform RISCV_PLAT "Select the platform for the architecture"
    "spike;KernelPlatformSpike;PLAT_SPIKE;KernelSel4ArchRISCV"
)

if(KernelArchRISCV)
    config_set(KernelSel4Arch SEL4_ARCH "${KernelRISCVSel4Arch}")
    set(KernelIPCBufferLocation "tp_register" CACHE STRING "")
endif()

if(KernelSel4ArchRISCV)
    set_kernel_64()
endif()

# Include all the platforms. For all of the common variables we set a default value here
# and let the platforms override them.
include(src/plat/spike/config.cmake)

config_choice(KernelIPCBufferLocation KERNEL_IPC_BUFFER_LOCATION
    "Controls how the location of the IPC buffer is provided to the user \
    tp_register-> Put the address of the IPC buffer in the user readable/writeable \
        tp register. When enabled this has the result of the kernel overwriting \
        any value the user writes to this register."
    "tp_register;KernelIPCBufferTP;IPC_BUF_IN_RISCV_TP_REGISTER;KernelArchRISCV"
)

add_sources(
    DEP "KernelArchRISCV"
    PREFIX src/arch/riscv
    ASMFILES
        head.S
        halt.S
        traps.S
    CFILES
        c_traps.c
        idle.c
        api/faults.c
        api/benchmark.c
        kernel/boot.c
        kernel/thread.c
        kernel/vspace64.c
        machine/capdl.c
        machine/debug.c
        machine/hardware.c
        machine/registerset.c
        model/statedata.c
        object/interrupt.c
        object/objecttype.c
        object/tcb.c
        smp/ipi.c
)

add_bf_source_old("KernelArchRISCV" "structures.bf" "include/arch/riscv" "arch/object")
add_bf_source_old("KernelArchRISCV" "hardware.bf" "include/plat/${KernelPlatform}" "plat/machine")
