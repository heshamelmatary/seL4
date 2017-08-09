void riscv_vm_contextSwitch(pte_t* cap_pd, asid_t asid)
{
    setCurrentPD(addrFromPPtr(cap_pd), asid);
}

void setHardwareASID(uint32_t asid)
{
}
