/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_KERNEL_VSPACE_H
#define __ARCH_KERNEL_VSPACE_H

#include <types.h>
#include <api/failures.h>
#include <object/structures.h>

cap_t create_it_address_space(cap_t root_cnode_cap, v_region_t it_v_reg);
void map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap, uint32_t ptLevel);
void map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap);
void map_kernel_window(void);
void map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights);
void activate_kernel_vspace(void);
void write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap);


/* ==================== BOOT CODE FINISHES HERE ==================== */
#define IT_ASID 1

void idle_thread(void);
#define idleThreadStart (&idle_thread)

struct lookupPTSlot_ret {
    exception_t status;
    uint32_t missingPTLevel;
    pte_t *ptSlot;
};

typedef struct lookupPTSlot_ret lookupPTSlot_ret_t;

struct findVSpaceForASID_ret {
    exception_t status;
    pte_t *vspace_root;
};
typedef struct findVSpaceForASID_ret findVSpaceForASID_ret_t;

void copyGlobalMappings(pte_t *newlvl1pt);
word_t* PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread);
lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr, uint32_t ptLevel);
exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType);
void unmapPageTable(asid_t, vptr_t vaddr, pte_t* pt, uint32_t ptLevel);
bool_t CONST isValidVTableRoot(cap_t cap);
exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap);
vm_rights_t CONST maskVMRights(vm_rights_t vm_rights,
                               seL4_CapRights_t cap_rights_mask);
exception_t decodeMMUInvocation(word_t label, unsigned int length, cptr_t cptr,
                                cte_t *cte, cap_t cap, extra_caps_t extraCaps,
                                word_t *buffer);
exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                          pte_t lvl1pt, pte_t *ptSlot);
exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot);
exception_t performPageInvocationMapPTE(cap_t cap, cte_t *ctSlot,
                                        pte_t pte, pte_range_t pte_entries);
exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot);
void setVMRoot(tcb_t *tcb);
#endif