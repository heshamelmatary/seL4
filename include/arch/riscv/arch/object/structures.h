/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_OBJECT_STRUCTURES_H
#define __ARCH_OBJECT_STRUCTURES_H

#ifndef __ASSEMBLER__
#include <assert.h>
#include <util.h>
#include <api/types.h>
#include <arch/types.h>
#include <arch/object/structures_gen.h>
#include <arch/machine/hardware.h>
#include <arch/machine/registerset.h>

#define TCB_CNODE_RADIX 4
#define TCB_BLOCK_SIZE_BITS (TCB_SIZE_BITS + 1)
#define tcbArchCNodeEntries tcbCNodeEntries

enum asidSizeConstants {
    asidHighBits = 7,
    asidLowBits = 10
};

struct asid_pool {
    void* array[BIT(asidLowBits)];
};


typedef struct asid_pool asid_pool_t;

#define ASID_POOL_BITS      asidLowBits
#define ASID_POOL_SIZE_BITS (ASID_POOL_BITS + WORD_SIZE_BITS)
#define ASID_POOL_PTR(r)    ((asid_pool_t*)r)
#define ASID_POOL_REF(p)    ((word_t)p)
#define ASID_BITS           (asidHighBits + asidLowBits)
#define nASIDPools          BIT(asidHighBits)
#define ASID_LOW(a)         (a & MASK(asidLowBits))
#define ASID_HIGH(a)        ((a >> asidLowBits) & MASK(asidHighBits))

typedef struct arch_tcb {
    /* saved user-level context of thread (72 bytes) */
    user_context_t tcbContext;
} arch_tcb_t;

enum vm_rights {
    VMNoAccess = 0,
    VMKernelOnly = 1,
    VMReadOnly = 2,
    VMReadWrite = 3
};
typedef uint32_t vm_rights_t;

typedef pde_t vspace_root_t;

#define PDE_SIZE_BITS 2
#define PDE_PTR(r) ((pde_t *)(r))
#define PDE_REF(p) ((unsigned int)(p))

#define PDE_PTR_PTR(r) ((pde_t **)r)

#define PD_BITS 10
#define PD_SIZE_BITS (PDE_SIZE_BITS + PD_BITS)
#define PD_PTR(r) ((pde_t *)(r))
#define PD_REF(p) ((void *)(p))

/* Page directory entries (PDEs) */
enum pde_type {
    PDEInvalid = 0,
    PDECoarse  = 1,
    PDEMapping = 2
};
typedef uint32_t pde_type_t;


#define PTE_SIZE_BITS 2
#define PTE_PTR(r) ((pte_t *)(r))
#define PTE_REF(p) ((unsigned int)(p))

#define PT_BITS 8
#define PT_SIZE_BITS 12
#define PT_PTR(r) ((pte_t *)(r))
#define PT_REF(p) ((uint64_t)(p))

#define WORD_SIZE_BITS 2
#define WORD_BITS   (8 * sizeof(word_t))
#define WORD_PTR(r) ((word_t *)(r))
#define WORD_REF(p) ((uint64_t)(p))

struct user_data {
    word_t words[BIT(RISCV_4K_Page) / sizeof(word_t)];
};

typedef struct user_data user_data_t;

#define HW_ASID_SIZE_BITS 1

#define FMAPPED_OBJECT_LOW(a) ( ((a) >> 10) & MASK(2))
#define FMAPPED_OBJECT_HIGH(a) ( ((a) >> 12)  & MASK(20))

static inline bool_t CONST
cap_get_archCapIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {

    case cap_frame_cap:
        return true;

    case cap_page_table_cap:
        return true;

    case cap_lvl2_page_table_cap:
        return true;

    case cap_page_directory_cap:
        return true;

    case cap_asid_control_cap:
        return false;

    case cap_asid_pool_cap:
        return true;

    default:
        fail("Invalid arch cap type");
    }
}

static inline uint32_t CONST
cap_frame_cap_get_capFMappedObject(cap_t cap)
{
    //return (cap_frame_cap_get_capFMappedObjectHigh(cap) << 12);
    return (cap_frame_cap_get_capFMappedAddress(cap) << 12);
}

static inline cap_t CONST
cap_frame_cap_set_capFMappedObject(cap_t cap, uint32_t val)
{
    return cap_frame_cap_set_capFMappedObject(cap, FMAPPED_OBJECT_HIGH(val));
}

static inline void
cap_frame_cap_ptr_set_capFMappedObject(cap_t *cap, paddr_t val)
{
    cap_frame_cap_ptr_set_capFMappedObject(cap, val);
}

static inline unsigned int CONST
cap_get_archCapSizeBits(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {
    case cap_frame_cap:
        return pageBitsForSize(cap_frame_cap_get_capFSize(cap));

    case cap_page_table_cap:
    case cap_lvl2_page_table_cap:
        return PT_SIZE_BITS;

    case cap_page_directory_cap:
        return PD_SIZE_BITS;

    case cap_asid_control_cap:
        return 0;

    case cap_asid_pool_cap:
        return seL4_ASIDPoolBits;

    default:
        assert(!"Unknown cap type");
        /* Unreachable, but GCC can't figure that out */
        return 0;
    }
}

static inline void * CONST
cap_get_archCapPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {

    case cap_frame_cap:
        return (void *)(cap_frame_cap_get_capFBasePtr(cap));

    case cap_page_table_cap:
        return PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));

    case cap_lvl2_page_table_cap:
        return PT_PTR(cap_lvl2_page_table_cap_get_capLVL2PTBasePtr(cap));

    case cap_page_directory_cap:
        return PD_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));

    case cap_asid_control_cap:
        return NULL;

    case cap_asid_pool_cap:
        return ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap));

    default:
        assert(!"Unknown cap type");
        /* Unreachable, but GCC can't figure that out */
        return NULL;
    }
}
#endif /* !__ASSEMBLER__  */
#endif
