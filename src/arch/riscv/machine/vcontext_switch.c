void riscv_vm_contextSwitch(pde_t* cap_pd)
{
    setCurrentPD(addrFromPPtr(cap_pd));
}

void setHardwareASID(uint32_t asid)
{
}
