/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <types.h>
#include <benchmark/benchmark.h>
#include <api/failures.h>
#include <api/syscall.h>
#include <kernel/boot.h>
#include <kernel/cspace.h>
#include <kernel/thread.h>
#include <machine/io.h>
#include <model/preemption.h>
#include <model/statedata.h>
#include <object/cnode.h>
#include <object/untyped.h>
#include <arch/api/invocation.h>
#include <arch/kernel/vspace.h>
#include <arch/linker.h>
#include <arch/object/tcb.h>
#include <plat/machine/devices.h>
#include <plat/machine/hardware.h>
#include <kernel/stack.h>

struct resolve_ret {
    paddr_t frameBase;
    vm_page_size_t frameSize;
    bool_t valid;
};
typedef struct resolve_ret resolve_ret_t;

static resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr);
static exception_t performPageGetAddress(void *vbase_ptr);

static bool_t PURE pteCheckIfMapped(pte_t *pte);
static bool_t PURE pdeCheckIfMapped(pde_t *pde);


static word_t CONST
APFromVMRights(vm_rights_t vm_rights)
{
    switch (vm_rights) {
    case VMNoAccess:
        return 0;

    case VMKernelOnly:
        return pte_type_new(0, /* user */
                            1, /* execute */
                            1, /* write */
                            1  /* read */
                           ).words[0];

    case VMReadOnly:
        return pte_type_new(1, /* user */
                            1, /* execute */
                            0, /* write */
                            1  /* read */
                           ).words[0];

    case VMReadWrite:
        return pte_type_new(1, /* user */
                            1, /* execute */
                            1, /* write */
                            1  /* read */
                           ).words[0];

    default:
        fail("Invalid VM rights");
    }
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_CODE void
map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
    uint32_t idx = SV39_GET_LVL3_PT_INDEX(vaddr);

    /* vaddr lies in the region the global PT covers */
    assert(vaddr >= PPTR_TOP);
    l3pt[idx] =    pte_new(
                       paddr >> RISCV_4K_PageBits,
                       0,  /* sw */
                       1,  /* dirty */
                       1,  /* read */
                       1,  /* global */
                       APFromVMRights(vm_rights), /* type */
                       1 /* valid */
                   );
}

BOOT_CODE VISIBLE void
map_kernel_window(uint64_t sbi_pt)
{
    pde_t    pde;
    uint64_t  i, pt_index;
    uint32_t temp;
    /* mapping of kernelBase (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 4MB */

    /* Note, this assumes the kernel is mapped to 0xFFFFFFFF80000000 */
    /* 256 MiB kernel mapping (128 PTE * 2MiB per entry) */
    l1pt[SV39_GET_LVL1_PT_INDEX(kernelBase)] =  pde_new(
                                                    (addrFromPPtr(l2pt) >> RISCV_4K_PageBits),
                                                    0,  /* sw */
                                                    1,  /* dirty */
                                                    1,  /* read */
                                                    1,  /* global */
                                                    pte_type_new(0, 0, 0, 0).words[0],
                                                    1 /* valid */
                                                );

    /* Map the kernel with 2M pages */
    for (i = 0, pt_index = 0; i < BIT(RISCV_2M_PageBits) * 127; i += BIT(RISCV_2M_PageBits), pt_index++) {
        /* The first two bits are always 0b11 since the MSB is 0xF */
        l2pt[pt_index] = pte_new(
                             (physBase + i) >> RISCV_4K_PageBits,
                             0,  /* sw */
                             1,  /* dirty */
                             1,  /* read */
                             1,  /* global */
                             pte_type_new(0, 1, 1, 1).words[0],
                             1 /* valid */
                         );
    }

    assert((physBase + i) == PADDR_TOP);

    /* Map last 2MiB Page to page tables - */
    l2pt[127] =  pte_new(
                     (addrFromPPtr(l3pt) >> RISCV_4K_PageBits),
                     0,  /* sw */
                     1,  /* dirty */
                     1,  /* read */
                     1,  /* global */
                     pte_type_new(0, 0, 0, 0).words[0],
                     1 /* valid */
                 );

    /* now start initialising the page table */
    memzero(l3pt, BIT(RISCV_4K_PageBits));

    /* map global page */
    map_kernel_frame(
        addrFromPPtr(riscvKSGlobalsFrame),
        PPTR_GLOBALS_PAGE,
        VMReadOnly);

    /* map kernel stack */
    map_kernel_frame(
        addrFromPPtr(kernel_stack_alloc),
        PPTR_KERNEL_STACK,
        VMKernelOnly);

    setCurrentPD(addrFromPPtr(l1pt), 0);
}

BOOT_CODE void
map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pde_t* pd   = PDE_PTR(pptr_of_cap(vspace_cap));
    pte_t* pt   = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);

    uint32_t pdIndex = SV39_GET_LVL1_PT_INDEX(vptr);
    uint32_t ptlvl2Index = SV39_GET_LVL2_PT_INDEX(vptr);

    pte_t* lvl2pt = ptrFromPAddr(((*((uint64_t *)(pd + pdIndex)) >> 10) << RISCV_4K_PageBits));

    pte_t* targetSlot = lvl2pt + ptlvl2Index;

    *targetSlot = pte_new(
                      (addrFromPPtr(pt) >> RISCV_4K_PageBits),
                      0, /* sw */
                      1, /* dirty */
                      1, /* read */
                      0,  /* global */
                      pte_type_new(0, 0, 0, 0).words[0], /* type */
                      1 /* valid */
                  );
    asm volatile ("sfence.vma");
}

BOOT_CODE void
map_it_lvl2_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pde_t* pd   = PDE_PTR(pptr_of_cap(vspace_cap));
    pte_t* pt   = PTE_PTR(cap_lvl2_page_table_cap_get_capLVL2PTBasePtr(pt_cap));
    vptr_t vptr = cap_lvl2_page_table_cap_get_capLVL2PTMappedAddress(pt_cap);
    uint32_t pdIndex = SV39_GET_LVL1_PT_INDEX(vptr);

    pte_t* targetSlot = pd + pdIndex;

    *targetSlot = pte_new(
                      (addrFromPPtr(pt) >> RISCV_4K_PageBits),
                      0, /* sw */
                      1, /* dirty */
                      1, /* read */
                      0,  /* global */
                      pte_type_new(0, 0, 0, 0).words[0], /* type */
                      1 /* valid */
                  );
    asm volatile ("sfence.vma");
}

BOOT_CODE void
map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap)
{
    pde_t* pd   = PDE_PTR(pptr_of_cap(vspace_cap));
    pte_t* frame_pptr   = PTE_PTR(cap_frame_cap_get_capFBasePtr(frame_cap));
    vptr_t frame_vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);
    uint32_t pdIndex = SV39_GET_LVL1_PT_INDEX(frame_vptr);
    uint32_t ptlvl2Index = SV39_GET_LVL2_PT_INDEX(frame_vptr);
    uint32_t ptlvl3Index = SV39_GET_LVL3_PT_INDEX(frame_vptr);

    pte_t* lvl2pt = ptrFromPAddr((*((uint64_t *)(pd + pdIndex)) >> 10) << RISCV_4K_PageBits);
    pte_t* lvl3pt = ptrFromPAddr(((*((uint64_t *)(lvl2pt + ptlvl2Index)) >> 10) << RISCV_4K_PageBits));

    pte_t* targetSlot = lvl3pt + ptlvl3Index;

    *targetSlot = pte_new(
                      (pptr_to_paddr(frame_pptr) >> RISCV_4K_PageBits),
                      0, /* sw */
                      1, /* dirty */
                      1, /* read */
                      0,  /* global */
                      APFromVMRights(VMReadWrite), /* type */
                      1 /* valid */
                  );
    asm volatile ("sfence.vma");
}

BOOT_CODE void
unmap_it_frame_cap(cap_t vspace_cap, cap_t frame_cap)
{
    pde_t* pd   = PDE_PTR(pptr_of_cap(vspace_cap));
    pte_t* frame_pptr   = PTE_PTR(cap_frame_cap_get_capFBasePtr(frame_cap));
    vptr_t frame_vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);
    uint32_t pdIndex = SV39_GET_LVL1_PT_INDEX(frame_vptr);
    uint32_t ptlvl2Index = SV39_GET_LVL2_PT_INDEX(frame_vptr);
    uint32_t ptlvl3Index = SV39_GET_LVL3_PT_INDEX(frame_vptr);

    pte_t* lvl2pt = ptrFromPAddr((*((uint64_t *)(pd + pdIndex)) >> 10) << RISCV_4K_PageBits);
    pte_t* lvl3pt = ptrFromPAddr(((*((uint64_t *)(lvl2pt + ptlvl2Index)) >> 10) << RISCV_4K_PageBits));

    pte_t* targetSlot = lvl3pt + ptlvl3Index;

    *targetSlot = pte_new(
                      (pptr_to_paddr(frame_pptr) >> RISCV_4K_PageBits),
                      0, /* sw */
                      0, /* dirty */
                      0, /* read */
                      0,  /* global */
                      APFromVMRights(VMReadWrite), /* type */
                      0 /* valid */
                  );
    asm volatile ("sfence.vma");
}

BOOT_CODE cap_t
create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_frame_cap_new(
                    asidInvalid,                     /* capFMappedASID       */
                    pptr,                            /* capFBasePtr          */
                    0,                               /* capFSize             */
                    0,                               /* capFTMapype          */
                    0,                               /* capFMappedAddress    */
                    0,                               /* capFVMRights         */
                    0
                );

    return cap;
}

/* Create a page table for the initial thread */
static BOOT_CODE cap_t
create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    return cap;
}

static BOOT_CODE cap_t
create_it_lvl2_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_lvl2_page_table_cap_new(
              asid,   /* capLVL2PTMappedASID      */
              pptr,   /* capLVL2PTBasePtr         */
              1,      /* capLVL2PTIsMapped        */
              vptr    /* capLVL2PTMappedAddress   */
          );
    map_it_lvl2_pt_cap(vspace_cap, cap);
    return cap;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t
create_it_address_space(cap_t root_cnode_cap, v_region_t it_v_reg)
{
    cap_t      pd_cap;
    vptr_t     pt_vptr;
    pptr_t     pt_pptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;
    pptr_t pd_pptr;

    /* create 1st level page table obj and cap */
    pd_pptr = alloc_region(PD_SIZE_BITS);

    if (!pd_pptr) {
        return cap_null_cap_new();
    }
    memzero(PDE_PTR(pd_pptr), 1 << PD_SIZE_BITS);

    copyGlobalMappings(PDE_PTR(pd_pptr));

    pd_cap =
        cap_page_directory_cap_new(
            IT_ASID,            /* capPDMappedASID    */
            (word_t) pd_pptr,   /* capPDBasePtr       */
            1,                  /* capPDIsMapped      */
            (word_t) pd_pptr    /* capPDMappedAddress */
        );

    write_slot(SLOT_PTR(pptr_of_cap(root_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all 2nd level PT objs and caps necessary to cover userland image */
    slot_pos_before = ndks_boot.slot_pos_cur;

    for (pt_vptr = ROUND_DOWN(it_v_reg.start, RISCV_1G_PageBits);
            pt_vptr < it_v_reg.end;
            pt_vptr += BIT(RISCV_1G_PageBits)) {
        pt_pptr = alloc_region(PT_SIZE_BITS);

        if (!pt_pptr) {
            return cap_null_cap_new();
        }

        memzero(PTE_PTR(pt_pptr), 1 << PT_SIZE_BITS);
        if (!provide_cap(root_cnode_cap,
                         create_it_lvl2_pt_cap(pd_cap, pt_pptr, pt_vptr, IT_ASID))
           ) {
            return cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    for (pt_vptr = ROUND_DOWN(it_v_reg.start, RISCV_2M_PageBits);
            pt_vptr < it_v_reg.end;
            pt_vptr += BIT(RISCV_2M_PageBits)) {
        pt_pptr = alloc_region(PT_SIZE_BITS);
        if (!pt_pptr) {
            return cap_null_cap_new();
        }
        memzero(PT_PTR(pt_pptr), 1 << PT_SIZE_BITS);
        if (!provide_cap(root_cnode_cap,
                         create_it_pt_cap(pd_cap, pt_pptr, pt_vptr, IT_ASID))
           ) {
            return cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    setCurrentPD(addrFromPPtr(pd_pptr), IT_ASID);
    return pd_cap;
}

BOOT_CODE void
activate_global_pd(void)
{
    setCurrentPD(addrFromPPtr(l1pt), 0);
}

BOOT_CODE void
write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t* ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[IT_ASID] = PDE_PTR(pptr_of_cap(it_pd_cap));
    riscvKSASIDTable[IT_ASID >> asidLowBits] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_pool_t*        poolPtr;
    pde_t*     vspace_root;

    poolPtr = riscvKSASIDTable[asid >> asidLowBits];
    if (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_root_new();

        ret.vspace_root = NULL;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        return ret;
    }

    vspace_root = poolPtr->array[asid & MASK(asidLowBits)];
    if (!vspace_root) {
        current_lookup_fault = lookup_fault_invalid_root_new();

        ret.vspace_root = NULL;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        return ret;
    }

    ret.vspace_root = vspace_root;
    ret.status = EXCEPTION_NONE;
    return ret;
}

bool_t CONST isVTableRoot(cap_t cap)
{
    return cap_get_capType(cap) == cap_page_directory_cap;
}

bool_t CONST isValidNativeRoot(cap_t cap)
{
    return isVTableRoot(cap) &&
           cap_page_directory_cap_get_capPDIsMapped(cap);
}

pde_t *getValidNativeRoot(cap_t vspace_cap)
{
    if (isValidNativeRoot(vspace_cap)) {
        return PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(vspace_cap));
    }
    return NULL;
}

void
copyGlobalMappings(pde_t *newPD)
{
    unsigned int i;
    pde_t *global_pd = l1pt;

    for (i = SV39_GET_LVL1_PT_INDEX(kernelBase); i < 512; i++) {
        newPD[i] = global_pd[i];
    }
}

word_t * PURE
lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_bufferPtr;
    cap_t bufferCap;
    vm_rights_t vm_rights;

    w_bufferPtr = thread->tcbIPCBuffer;
    bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

    if (unlikely(cap_get_capType(bufferCap) != cap_frame_cap &&
                 cap_get_capType(bufferCap) != cap_frame_cap)) {
        return NULL;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(bufferCap);
    if (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        unsigned int pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(cap_frame_cap_get_capFSize(bufferCap));
        return (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } else {
        return NULL;
    }
}

lookupPDSlot_ret_t CONST
lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    lookupPDSlot_ret_t ret;
    unsigned int pdIndex;
    pdIndex = SV39_GET_LVL1_PT_INDEX(vptr);
    ret.status = EXCEPTION_NONE;
    ret.pdSlot = pd + pdIndex;
    return ret;
}

static lookupLVL2PTSlot_ret_t CONST
lookupLVL2PTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPDSlot_ret_t lvl1Slot = lookupPDSlot(pd, vptr);
    lookupLVL2PTSlot_ret_t ret;

    if (lvl1Slot.status != EXCEPTION_NONE) {
        ret.ptSlot = NULL;
        ret.status = lvl1Slot.status;
        return ret;
    }

    if (*((uint64_t *)lvl1Slot.pdSlot) == NULL) {
        ret.ptSlot = NULL;
        current_lookup_fault = lookup_fault_missing_capability_new(30);
        ret.status = EXCEPTION_LOOKUP_FAULT;
        return ret;
    } else {
        uint32_t pdIndex = SV39_GET_LVL1_PT_INDEX(vptr);
        uint32_t ptlvl2Index = SV39_GET_LVL2_PT_INDEX(vptr);

        pte_t* lvl2pt = ptrFromPAddr(((*((uint64_t *)(lvl1Slot.pdSlot)) >> 10) << RISCV_4K_PageBits));

        ret.ptSlot = lvl2pt + ptlvl2Index;
        ret.status = EXCEPTION_NONE;
        return ret;
    }
}

lookupPTSlot_ret_t
lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupLVL2PTSlot_ret_t lvl2Slot = lookupLVL2PTSlot(pd, vptr);
    lookupPTSlot_ret_t ret;

    if (lvl2Slot.status != EXCEPTION_NONE) {
        ret.ptSlot = NULL;
        ret.status = lvl2Slot.status;
        return ret;
    }

    if (*((uint64_t *)lvl2Slot.ptSlot) == NULL ) {
        current_lookup_fault = lookup_fault_missing_capability_new(21);

        ret.ptSlot = NULL;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        return ret;
    } else {
        uint32_t ptlvl3Index = SV39_GET_LVL3_PT_INDEX(vptr);

        pte_t* lvl3pt = ptrFromPAddr(((*((uint64_t *)(lvl2Slot.ptSlot)) >> 10) << RISCV_4K_PageBits));

        ret.ptSlot = lvl3pt + ptlvl3Index;
        ret.status = EXCEPTION_NONE;
        return ret;
    }
}

exception_t
handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint32_t addr;
    uint32_t fault;

    addr = read_csr(sbadaddr);

    switch (vm_faultType) {
    case RISCVLoadPageFault:
    case RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        return EXCEPTION_FAULT;
    case RISCVStorePageFault:
    case RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        return EXCEPTION_FAULT;
    case RISCVInstructionPageFault:
    case RISCVInstructionAccessFault:
        setRegister(thread, SEPC, getRegister(thread, SEPC));
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        return EXCEPTION_FAULT;

    default:
        fail("Invalid VM fault type");
    }
}

void deleteASIDPool(asid_t asid_base, asid_pool_t* pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    assert(IS_ALIGNED(asid_base, asidLowBits));

    if (riscvKSASIDTable[asid_base >> asidLowBits] == pool) {
        riscvKSASIDTable[asid_base >> asidLowBits] = NULL;
        setVMRoot(ksCurThread);
    }
}

exception_t performASIDControlInvocation(void* frame, cte_t* slot, cte_t* parent, asid_t asid_base)
{
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, 1 << pageBitsForSize(RISCV_4K_Page));
    cteInsert(
        cap_asid_pool_cap_new(
            asid_base,          /* capASIDBase  */
            WORD_REF(frame)     /* capASIDPool  */
        ),
        parent,
        slot
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    assert((asid_base & MASK(asidLowBits)) == 0);
    riscvKSASIDTable[asid_base >> asidLowBits] = (asid_pool_t*)frame;

    return EXCEPTION_NONE;
}

exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t* poolPtr, cte_t* vspaceCapSlot)
{
    cap_t cap = vspaceCapSlot->cap;
    cap = cap_page_directory_cap_set_capPDMappedASID(cap, asid);
    cap = cap_page_directory_cap_set_capPDIsMapped(cap, 1);
    vspaceCapSlot->cap = cap;
    poolPtr->array[asid & MASK(asidLowBits)] = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(vspaceCapSlot->cap));

    return EXCEPTION_NONE;
}

void hwASIDInvalidate(asid_t asid)
{
    /* Not implemented */
    return;
}

void deleteASID(asid_t asid, pde_t *vspace)
{
    asid_pool_t* poolPtr;

    poolPtr = riscvKSASIDTable[asid >> asidLowBits];
    hwASIDInvalidate(asid);
    if (poolPtr != NULL && poolPtr->array[asid & MASK(asidLowBits)] == vspace) {
        poolPtr->array[asid & MASK(asidLowBits)] = NULL;
        setVMRoot(ksCurThread);
    }
}

pde_t *
pageTableMapped(asid_t asid, vptr_t vaddr, pte_t* pt)
{
    findVSpaceForASID_ret_t find_ret;
    lookupLVL2PTSlot_ret_t lu_lvl2pt_ret;
    pde_t pde;
    pte_t *lvl2_pte;

    unsigned int pdIndex;

    find_ret = findVSpaceForASID(asid);
    if (unlikely(find_ret.status != EXCEPTION_NONE)) {
        return NULL;
    }

    lu_lvl2pt_ret = lookupLVL2PTSlot(find_ret.vspace_root, vaddr);
    if (unlikely(lu_lvl2pt_ret.status != EXCEPTION_NONE)) {
        return NULL;
    }

    lvl2_pte = lu_lvl2pt_ret.ptSlot;

    pte_t* pt_from_hw = ptrFromPAddr((lvl2_pte->words[0] >> 10) << RISCV_4K_PageBits);

    if (pt_from_hw == pt) {
        return pt_from_hw;
    } else {
        return NULL;
    }
}

void
unmapPageTable(asid_t asid, vptr_t vaddr, pte_t* pt)
{
    pte_t *lvl2pt, *lvl2ptSlot;
    unsigned int pdIndex;

    lvl2pt = pageTableMapped (asid, vaddr, pt);

    if (likely(lvl2pt != NULL)) {

        lvl2ptSlot = lvl2pt;

        *lvl2ptSlot = pte_new(
                          0,  /* phy_address */
                          0,  /* sw */
                          0,  /* dirty */
                          0,  /* read */
                          0,  /* global */
                          0,  /* type */
                          0  /* valid */
                      );
        asm volatile ("sfence.vma");
    }
}

static pte_t pte_pte_invalid_new(void)
{
    pte_t invalid_pte = (pte_t) {
        0
    };
}

void
unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, void *pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;
    cap_t               threadRoot;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    find_ret = findVSpaceForASID(asid);
    if (find_ret.status != EXCEPTION_NONE) {
        return;
    }

    if (page_size == RISCV_4K_Page) {
        paddr_t addr = addrFromPPtr(pptr);
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(find_ret.vspace_root, vptr);
        if (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            return;
        }

        *((uint64_t *)lu_ret.ptSlot) = 0;
    } else if (page_size == RISCV_2M_Page) {
        lookupLVL2PTSlot_ret_t lu_ret;

        lu_ret = lookupLVL2PTSlot(find_ret.vspace_root, vptr);
        *((uint64_t *) lu_ret.ptSlot) = 0;
    } else if (page_size == RISCV_1G_Page) {
        lookupPDSlot_ret_t lu_ret;

        lu_ret = lookupPDSlot(find_ret.vspace_root, vptr);
        *((uint64_t *) lu_ret.pdSlot) = 0;
    }
    asm volatile ("sfence.vma");
}

void
setVMRoot(tcb_t *tcb)
{
    cap_t threadRoot;
    asid_t asid;
    pde_t *pd;
    findVSpaceForASID_ret_t  find_ret;

    threadRoot = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    if (cap_get_capType(threadRoot) != cap_page_directory_cap) {
        setCurrentPD(addrFromPPtr(l1pt), 0);
        return;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(threadRoot));

    asid = cap_page_directory_cap_get_capPDMappedASID(threadRoot);
    find_ret = findVSpaceForASID(asid);
    if (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_root != pd)) {
        setCurrentPD(addrFromPPtr(l1pt), asid);
        return;
    }

    riscv_vm_contextSwitch(pd, asid);
}

bool_t CONST
isValidVTableRoot(cap_t cap)
{
    return cap_get_capType(cap) == cap_page_directory_cap;
}

exception_t
checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    if (unlikely(cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        return EXCEPTION_SYSCALL_ERROR;
    }

    if (unlikely(vptr & MASK(9))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (int)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        return EXCEPTION_SYSCALL_ERROR;
    }

    return EXCEPTION_NONE;
}

vm_rights_t CONST
maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    if (vm_rights == VMNoAccess) {
        return VMNoAccess;
    }
    if (vm_rights == VMReadOnly &&
            seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        return VMReadOnly;
    }
    if (vm_rights == VMReadWrite &&
            seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        if (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            return VMReadOnly;
        } else {
            return VMReadWrite;
        }
    }
    return VMKernelOnly;
}

/* The rest of the file implements the RISCV object invocations */

static pte_t CONST
makeUserPTE(vm_page_size_t page_size, paddr_t paddr, vm_rights_t vm_rights)
{
    pte_t pte;

    switch (page_size) {
    case RISCV_4K_Page:
    case RISCV_2M_Page:
    case RISCV_1G_Page:

        pte = pte_new(
                  paddr >> RISCV_4K_PageBits,
                  0, /* sw */
                  1, /* dirty */
                  1, /* read */
                  0,  /* global */
                  APFromVMRights(vm_rights), /* type */
                  1 /* valid */
              );
        break;
    default:
        fail("Invalid PTE frame type");
    }
    return pte;
}

static inline bool_t CONST
checkVPAlignment(vm_page_size_t sz, word_t w)
{
    return (w & MASK(pageBitsForSize(sz))) == 0;
}

static exception_t
decodeRISCVLVL2PageTableInvocation(word_t label, unsigned int length,
                                   cte_t *cte, cap_t cap, extra_caps_t extraCaps,
                                   word_t *buffer)
{
    word_t vaddr, pdIndex;
    vm_attributes_t attr;
    cap_t pdCap;
    pde_t *pd, *pdSlot;
    pde_t pde;
    paddr_t paddr;
    lookupPDSlot_ret_t lu_ret;
    asid_t          asid;

    if (label == RISCVLVL2PageTableUnmap) {
        setThreadState(ksCurThread, ThreadState_Restart);
        return performPageTableInvocationUnmap (cap, cte);
    }

    if (unlikely(label != RISCVLVL2PageTableMap)) {
        current_syscall_error.type = seL4_IllegalOperation;
        return EXCEPTION_SYSCALL_ERROR;
    }

    if (unlikely(length < 2 || extraCaps.excaprefs[0] == NULL)) {
        current_syscall_error.type = seL4_TruncatedMessage;
        return EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
    attr = vmAttributesFromWord(getSyscallArg(1, buffer));
    pdCap = extraCaps.excaprefs[0]->cap;

    if (!isValidNativeRoot(pdCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        return EXCEPTION_SYSCALL_ERROR;
    }

    if (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        return EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCap));
    asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

    if (unlikely(vaddr >= kernelBase)) {
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 0;

        return EXCEPTION_SYSCALL_ERROR;
    }

    {
        findVSpaceForASID_ret_t find_ret;

        find_ret = findVSpaceForASID(asid);
        if (find_ret.status != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            return EXCEPTION_SYSCALL_ERROR;
        }

        if (find_ret.vspace_root != pd) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 1;

            return EXCEPTION_SYSCALL_ERROR;
        }
    }

    lu_ret = lookupPDSlot(pd, vaddr);

    if (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        return EXCEPTION_SYSCALL_ERROR;
    }

    pdSlot = lu_ret.pdSlot;

    if (unlikely( *((uint64_t *) pdSlot) != 0) ) {
        current_syscall_error.type = seL4_DeleteFirst;

        return EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_lvl2_page_table_cap_get_capLVL2PTBasePtr(cap)));

    pde = pde_new(
              (paddr >> RISCV_4K_PageBits),
              0, /* sw */
              1, /* dirty */
              1, /* read */
              0,  /* global */
              pte_type_new(0, 0, 0, 0).words[0], /* type */
              1 /* valid */
          );

    cap = cap_lvl2_page_table_cap_set_capLVL2PTIsMapped(cap, 1);
    cap = cap_lvl2_page_table_cap_set_capLVL2PTMappedASID(cap, asid);
    cap = cap_lvl2_page_table_cap_set_capLVL2PTMappedAddress(cap, vaddr);

    setThreadState(ksCurThread, ThreadState_Restart);
    return performPageTableInvocationMap(cap, cte, pde, pdSlot);

}

static exception_t
decodeRISCVPageTableInvocation(word_t label, unsigned int length,
                               cte_t *cte, cap_t cap, extra_caps_t extraCaps,
                               word_t *buffer)
{
    word_t vaddr, pdIndex;
    vm_attributes_t attr;
    cap_t pdCap;
    pde_t *pd, *pdSlot;
    pde_t pde;
    paddr_t paddr;
    lookupLVL2PTSlot_ret_t lu_ret;
    asid_t          asid;

    if (label == RISCVPageTableUnmap) {
        setThreadState(ksCurThread, ThreadState_Restart);
        return performPageTableInvocationUnmap (cap, cte);
    }

    if (unlikely(label != RISCVPageTableMap)) {
        current_syscall_error.type = seL4_IllegalOperation;
        return EXCEPTION_SYSCALL_ERROR;
    }

    if (unlikely(length < 2 || extraCaps.excaprefs[0] == NULL)) {
        current_syscall_error.type = seL4_TruncatedMessage;
        return EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
    attr = vmAttributesFromWord(getSyscallArg(1, buffer));
    pdCap = extraCaps.excaprefs[0]->cap;

    if (!isValidNativeRoot(pdCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        return EXCEPTION_SYSCALL_ERROR;
    }

    if (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;

        return EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCap));
    asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

    if (unlikely(vaddr >= kernelBase)) {
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 0;

        return EXCEPTION_SYSCALL_ERROR;
    }

    {
        findVSpaceForASID_ret_t find_ret;

        find_ret = findVSpaceForASID(asid);
        if (find_ret.status != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            return EXCEPTION_SYSCALL_ERROR;
        }

        if (find_ret.vspace_root != pd) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 1;

            return EXCEPTION_SYSCALL_ERROR;
        }
    }

    lu_ret = lookupLVL2PTSlot(pd, vaddr);

    if (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        return EXCEPTION_SYSCALL_ERROR;
    }

    pdSlot = lu_ret.ptSlot;

    if (unlikely( *((uint64_t *) pdSlot) != 0) ) {
        current_syscall_error.type = seL4_DeleteFirst;

        return EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));

    pde = pde_new(
              (paddr >> RISCV_4K_PageBits),
              0, /* sw */
              1, /* dirty */
              1, /* read */
              0,  /* global */
              pte_type_new(0, 0, 0, 0).words[0], /* type */
              1 /* valid */
          );

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, vaddr);

    setThreadState(ksCurThread, ThreadState_Restart);
    return performPageTableInvocationMap(cap, cte, pde, pdSlot);
}

struct create_mappings_pte_return {
    exception_t status;
    pte_t pte;
    pte_range_t pte_entries;
};
typedef struct create_mappings_pte_return create_mappings_pte_return_t;

struct create_mappings_pde_return {
    exception_t status;
    pde_t pde;
    pde_range_t pde_entries;
};
typedef struct create_mappings_pde_return create_mappings_pde_return_t;

static create_mappings_pte_return_t
createSafeMappingEntries_PTE
(paddr_t base, word_t vaddr, vm_page_size_t frameSize,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{
    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;

    unsigned int i;

    switch (frameSize) {

    case RISCV_4K_Page: {

        ret.pte_entries.base = 0;
        ret.pte_entries.length = 1;

        ret.pte = makeUserPTE(RISCV_4K_Page, base, vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        if (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            return ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;

        ret.status = EXCEPTION_NONE;
        return ret;

    }

    case RISCV_2M_Page: {
        ret.pte_entries.base = 0;
        ret.pte_entries.length = 1;

        ret.pte = makeUserPTE(RISCV_2M_Page, base, vmRights);

        lookupLVL2PTSlot_ret_t lu_lvl2_ret = lookupLVL2PTSlot(pd, vaddr);

        ret.pte_entries.base = lu_lvl2_ret.ptSlot;

        ret.status = EXCEPTION_NONE;
        return ret;
    }

    case RISCV_1G_Page: {
        ret.pte_entries.base = 0;
        ret.pte_entries.length = 1;

        ret.pte = makeUserPTE(RISCV_1G_Page, base, vmRights);

        lookupPDSlot_ret_t lu_pd_ret = lookupPDSlot(pd, vaddr);

        ret.pte_entries.base = lu_pd_ret.pdSlot;

        ret.status = EXCEPTION_NONE;
        return ret;
    }
    default:
        fail("Invalid or unexpected RISCV page type.");

    }
}

static exception_t
decodeRISCVFrameInvocation(word_t label, unsigned int length,
                           cte_t *cte, cap_t cap, extra_caps_t extraCaps,
                           word_t *buffer)
{
    switch (label) {
    case RISCVPageMap: {

        word_t vaddr, vtop, w_rightsMask;
        paddr_t frame_paddr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_page_size_t frameSize;
        vm_attributes_t attr;

        if (unlikely(length < 3 || extraCaps.excaprefs[0] == NULL)) {
            current_syscall_error.type =
                seL4_TruncatedMessage;

            return EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, buffer);
        w_rightsMask = getSyscallArg(1, buffer);
        attr = vmAttributesFromWord(getSyscallArg(2, buffer));
        pdCap = extraCaps.excaprefs[0]->cap;

        frameSize = cap_frame_cap_get_capFSize(cap);
        capVMRights = cap_frame_cap_get_capFVMRights(cap);

        if (unlikely(cap_frame_cap_get_capFMappedASID(cap)) != asidInvalid) {
            if (cap_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("RISCVPageMap: Trying to map the same frame cap to different vaddr %p", vaddr);
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalidCapNumber = 0;
                return EXCEPTION_SYSCALL_ERROR;
            }

        }

        if (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap)) {
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 1;

            return EXCEPTION_SYSCALL_ERROR;
        }

        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(
                         pdCap));

        lookupPTSlot_ret_t lu_ret;
        lookupLVL2PTSlot_ret_t lu_lvl2pt_ret;
        lookupPDSlot_ret_t lu_pd_ret;

        /* Check if this page is already mapped */
        if (frameSize == RISCV_4K_Page) {
            lu_ret = lookupPTSlot(pd, vaddr);

            if (unlikely(lu_ret.status != EXCEPTION_NONE)) {
                userError("RISCVPageMap: No PageTable for this page %p", vaddr);
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;
                return EXCEPTION_SYSCALL_ERROR;
            }
        } else if (frameSize == RISCV_2M_Page) {
            lu_lvl2pt_ret = lookupLVL2PTSlot(pd, vaddr);
            if (unlikely(lu_lvl2pt_ret.status != EXCEPTION_NONE)) {
                userError("RISCVPageMap: No PageTable for this page %p", vaddr);
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;
                return EXCEPTION_SYSCALL_ERROR;
            }

        } else if (frameSize == RISCV_1G_Page) {
            lu_pd_ret = lookupPDSlot(pd, vaddr);
            if (unlikely(lu_pd_ret.status != EXCEPTION_NONE)) {
                userError("RISCVPageMap: No PageTable for this page %p", vaddr);
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;
                return EXCEPTION_SYSCALL_ERROR;
            }
        }

        asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

        {
            findVSpaceForASID_ret_t find_ret;

            find_ret = findVSpaceForASID(asid);
            if (find_ret.status != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource = false;

                return EXCEPTION_SYSCALL_ERROR;
            }

            if (find_ret.vspace_root != pd) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalidCapNumber = 1;

                return EXCEPTION_SYSCALL_ERROR;
            }
        }

        vtop = vaddr + BIT(pageBitsForSize(frameSize)) - 1;

        if (unlikely(vtop >= kernelBase)) {
            current_syscall_error.type =
                seL4_InvalidArgument;
            current_syscall_error.invalidArgumentNumber = 0;

            return EXCEPTION_SYSCALL_ERROR;
        }

        vmRights =
            maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

        if (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            current_syscall_error.type =
                seL4_AlignmentError;

            return EXCEPTION_SYSCALL_ERROR;
        }

        frame_paddr = addrFromPPtr((void *)
                                   cap_frame_cap_get_capFBasePtr(cap));


        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap,  vaddr);

        switch (frameSize) {
        case RISCV_1G_Page:
        case RISCV_2M_Page:
        case RISCV_4K_Page: {
            create_mappings_pte_return_t map_ret;
            map_ret = createSafeMappingEntries_PTE(frame_paddr, vaddr,
                                                   frameSize, vmRights,
                                                   attr, pd);

            if (unlikely(map_ret.status != EXCEPTION_NONE)) {
                return map_ret.status;
            }

            setThreadState(ksCurThread, ThreadState_Restart);
            return performPageInvocationMapPTE(cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        }

        default:
            printf("error: Not a valid page size \n");
        }
    }

    case RISCVPageUnmap: {
        setThreadState(ksCurThread, ThreadState_Restart);
        return performPageInvocationUnmap(cap, cte);
    }

    case RISCVPageGetAddress: {

        /* Check that there are enough message registers */
        assert(n_msgRegisters >= 1);

        setThreadState(ksCurThread, ThreadState_Restart);
        return performPageGetAddress((void*)cap_frame_cap_get_capFBasePtr(cap));
    }

    }

}

static const resolve_ret_t default_resolve_ret_t;

static inline vptr_t
pageBase(vptr_t vaddr, vm_page_size_t size)
{
    return vaddr & ~MASK(pageBitsForSize(size));
}

static exception_t
decodeRISCVPageDirectoryInvocation
(
    word_t label,
    unsigned int length,
    cte_t* cte,
    cap_t cap,
    extra_caps_t extraCaps,
    word_t* buffer
)
{
    current_syscall_error.type = seL4_IllegalOperation;
    return EXCEPTION_SYSCALL_ERROR;
}

exception_t
decodeRISCVMMUInvocation(word_t label, unsigned int length, cptr_t cptr,
                         cte_t *cte, cap_t cap, extra_caps_t extraCaps,
                         word_t *buffer)
{
    switch (cap_get_capType(cap)) {

    case cap_page_directory_cap:
        return decodeRISCVPageDirectoryInvocation(label, length, cte, cap, extraCaps, buffer);

    case cap_lvl2_page_table_cap:
        return decodeRISCVLVL2PageTableInvocation(label, length, cte, cap, extraCaps, buffer);

    case cap_page_table_cap:
        return decodeRISCVPageTableInvocation(label, length, cte, cap, extraCaps, buffer);

    case cap_frame_cap:
        return decodeRISCVFrameInvocation(label, length, cte, cap, extraCaps, buffer);

    case cap_asid_control_cap: {
        word_t     i;
        asid_t           asid_base;
        word_t           index;
        word_t           depth;
        cap_t            untyped;
        cap_t            root;
        cte_t*           parentSlot;
        cte_t*           destSlot;
        lookupSlot_ret_t lu_ret;
        void*            frame;
        exception_t      status;

        if (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            return EXCEPTION_SYSCALL_ERROR;
        }

        if (length < 2 || extraCaps.excaprefs[0] == NULL
                || extraCaps.excaprefs[1] == NULL) {
            current_syscall_error.type = seL4_TruncatedMessage;
            return EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = extraCaps.excaprefs[0];
        untyped = parentSlot->cap;
        root = extraCaps.excaprefs[1]->cap;

        /* Find first free pool */
        for (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        if (i == nASIDPools) {
            /* no unallocated pool is found */
            current_syscall_error.type = seL4_DeleteFirst;

            return EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asidLowBits;


        if (cap_get_capType(untyped) != cap_untyped_cap ||
                cap_untyped_cap_get_capBlockSize(untyped) != seL4_ASIDPoolBits ||
                cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 1;

            return EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        if (status != EXCEPTION_NONE) {
            return status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(root, index, depth);
        if (lu_ret.status != EXCEPTION_NONE) {
            return lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        if (status != EXCEPTION_NONE) {
            return status;
        }

        setThreadState(ksCurThread, ThreadState_Restart);
        return performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    case cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t*       vspaceCapSlot;
        asid_pool_t* pool;
        word_t i;
        asid_t       asid;

        if (label != RISCVASIDPoolAssign) {
            current_syscall_error.type = seL4_IllegalOperation;

            return EXCEPTION_SYSCALL_ERROR;
        }
        if (extraCaps.excaprefs[0] == NULL) {
            current_syscall_error.type = seL4_TruncatedMessage;

            return EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = extraCaps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        if (!isVTableRoot(vspaceCap) ||
                cap_page_directory_cap_get_capPDMappedASID(vspaceCap) != asidInvalid) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 1;

            return EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[cap_asid_pool_cap_get_capASIDBase(cap) >> asidLowBits];
        if (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_root_new();
            return EXCEPTION_SYSCALL_ERROR;
        }

        if (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            return EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        for (i = 0; i < BIT(asidLowBits) && (asid + i == 0 || pool->array[i]); i++);

        if (i == BIT(asidLowBits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            return EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(ksCurThread, ThreadState_Restart);
        return performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }
    default:
        fail("Invalid arch cap type");
    }
}

exception_t
performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                              pde_t pde, pde_t *pdSlot)
{
    ctSlot->cap = cap;
    *pdSlot = pde;

    return EXCEPTION_NONE;
}

exception_t
performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    if (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt);
        clearMemory((void *)pt, RISCV_4K_PageBits);
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    return EXCEPTION_NONE;
}

static exception_t
performPageGetAddress(void *vbase_ptr)
{
    paddr_t capFBasePtr;

    /* Get the physical address of this frame. */
    capFBasePtr = addrFromPPtr(vbase_ptr);

    /* return it in the first message register */
    setRegister(ksCurThread, msgRegisters[0], capFBasePtr);
    setRegister(ksCurThread, msgInfoRegister,
                wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, 1)));

    return EXCEPTION_NONE;
}

static bool_t PURE
pteCheckIfMapped(pte_t *pte)
{
    return *((uint64_t *) pte) != 0;
}

exception_t performPageInvocationMapPTE(cap_t cap, cte_t *ctSlot,
                                        pte_t pte, pte_range_t pte_entries)
{
    unsigned int i;

    ctSlot->cap = cap;

    for (i = 0; i < pte_entries.length; i++) {
        pte_entries.base[i] = pte;
    }
    asm volatile ("sfence.vma");
    return EXCEPTION_NONE;
}

exception_t
performPageInvocationRemapPTE(asid_t asid, pte_t pte, pte_range_t pte_entries)
{
    word_t i, j UNUSED;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */

    j = pte_entries.length;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

    for (i = 0; i < pte_entries.length; i++) {
        pte_entries.base[i] = pte;
    }
    asm volatile ("sfence.vma");

    return EXCEPTION_NONE;
}

exception_t
performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{

    if (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        unmapPage(cap_frame_cap_get_capFSize(cap),
                  cap_frame_cap_get_capFMappedASID(cap),
                  cap_frame_cap_get_capFMappedAddress(cap),
                  cap_frame_cap_get_capFBasePtr(cap)
                 );
    }
    ctSlot->cap = cap_frame_cap_set_capFMappedAddress(ctSlot->cap, 0);
    ctSlot->cap = cap_frame_cap_set_capFMappedASID(ctSlot->cap, asidInvalid);
    return EXCEPTION_NONE;
}

void
Arch_userStackTrace(tcb_t *tptr)
{
    /* Not implemented */
    printf("Arch_userStackTrace not implemented\n");
    halt();
}
