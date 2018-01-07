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
void map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap);
void map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap);
void map_kernel_window(uint64_t sbi_pt);
void map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights);
void activate_global_pd(void);
void write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap);


/* ==================== BOOT CODE FINISHES HERE ==================== */

/* PD slot reserved for storing the PD's allocated hardware ASID */
#define PD_ASID_SLOT 0xff0

#define IT_ASID 1

void idle_thread(void);
#define idleThreadStart (&idle_thread)

enum pde_pte_tag {
    ME_PDE,
    ME_PTE
};
typedef uint32_t pde_pte_tag_t;

struct createMappingEntries_ret {
    exception_t status;
    pde_pte_tag_t tag;
    void *pde_pte_ptr;
    unsigned int offset;
    unsigned int size;
};
typedef struct createMappingEntries_ret createMappingEntries_ret_t;

struct lookupPTSlot_ret {
    exception_t status;
    pte_t *ptSlot;
};

typedef struct lookupPTSlot_ret lookupPTSlot_ret_t;

struct lookupLVL2PTSlot_ret {
    exception_t status;
    pte_t     *ptSlot;
};

typedef struct lookupLVL2PTSlot_ret lookupLVL2PTSlot_ret_t;

struct lookupPDSlot_ret {
    exception_t status;
    pte_t*      pdSlot;
};

typedef struct lookupPDSlot_ret lookupPDSlot_ret_t;

struct findVSpaceForASID_ret {
    exception_t status;
    pte_t *vspace_root;
};
typedef struct findVSpaceForASID_ret findVSpaceForASID_ret_t;

void copyGlobalMappings(pte_t *newPD);
word_t* PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread);
lookupPTSlot_ret_t lookupPTSlot(pte_t *pd, vptr_t vptr);
lookupPDSlot_ret_t CONST lookupPDSlot(pte_t *pd, vptr_t vptr);
exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType);
void unmapPageTable(asid_t, vptr_t vaddr, pte_t* pt);
bool_t CONST isValidVTableRoot(cap_t cap);
exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap);
vm_rights_t CONST maskVMRights(vm_rights_t vm_rights,
                               seL4_CapRights_t cap_rights_mask);
void flushPage(vm_page_size_t page_size, pte_t* pd, word_t vptr);
void flushTable(pte_t* pd, word_t vptr, pte_t* pt);
exception_t decodeMMUInvocation(word_t label, unsigned int length, cptr_t cptr,
                                cte_t *cte, cap_t cap, extra_caps_t extraCaps,
                                word_t *buffer);
exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                          pte_t pde, pte_t *pdSlot);
exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot);
exception_t performPageInvocationMapPTE(cap_t cap, cte_t *ctSlot,
                                        pte_t pte, pte_range_t pte_entries);
exception_t performPageInvocationMapPDE(cap_t cap, cte_t *ctSlot,
                                        pte_t pde, pde_range_t pde_entries);
exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot);
void setVMRoot(tcb_t *tcb);
#endif
