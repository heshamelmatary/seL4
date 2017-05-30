void riscv_vm_contextSwitch(pde_t* cap_pd, asid_t asid)
{
    setCurrentPD(addrFromPPtr(cap_pd), asid);
}

void setHardwareASID(uint32_t asid)
{
}
/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

