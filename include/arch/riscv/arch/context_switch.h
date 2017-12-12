
#pragma once
#include <arch/machine.h>

void riscv_vm_contextSwitch(pte_t* cap_pd, asid_t asid)
{
    setVSpaceRoot(addrFromPPtr(cap_pd), asid);
}

void setHardwareASID(uint32_t asid)
{
}
