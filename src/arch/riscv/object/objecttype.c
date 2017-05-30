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
#include <api/failures.h>
#include <kernel/vspace.h>
#include <object/structures.h>
#include <arch/machine.h>
#include <arch/model/statedata.h>
#include <arch/object/objecttype.h>

deriveCap_ret_t
Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {
    case cap_page_table_cap:
        ret.status = EXCEPTION_NONE;
        return ret;

    case cap_lvl2_page_table_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        return ret;

    case cap_page_directory_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        return ret;

    case cap_frame_cap:
        cap = cap_frame_cap_set_capFMappedAddress(cap, 0);
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        return ret;

    case cap_asid_control_cap:
    case cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        return ret;

    default:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        printf("Invalid arch cap type %d\n", cap_get_capType(cap));
        fail("Invalid arch cap type");
    }
}

cap_t CONST
Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    return cap;
}

cap_t CONST
Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    return cap;
}

cap_t
Arch_finaliseCap(cap_t cap, bool_t final)
{
    switch (cap_get_capType(cap)) {
    case cap_page_directory_cap:
        if (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        break;
    case cap_frame_cap:
        if (final) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (void *)cap_frame_cap_get_capFBasePtr(cap));
        }
        break;
    case cap_page_table_cap:
        if (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        break;
    }
    return cap_null_cap_new();
}

static cap_t CONST
resetMemMapping(cap_t cap)
{
    switch (cap_get_capType(cap)) {
    case cap_frame_cap:
        return cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
    case cap_page_table_cap:
        /* We don't need to worry about clearing ASID and Address here, only whether it is mapped */
        return cap_page_table_cap_set_capPTIsMapped(cap, 0);
    case cap_lvl2_page_table_cap:
        /* We don't need to worry about clearing ASID and Address here, only whether it is mapped */
        return cap_lvl2_page_table_cap_set_capLVL2PTIsMapped(cap, 0);
    case cap_page_directory_cap:
        /* We don't need to worry about clearing ASID and Address here, only whether it is mapped */
        return cap_page_directory_cap_set_capPDIsMapped(cap, 0);
    }
    return cap;
}

cap_t
Arch_recycleCap(bool_t is_final, cap_t cap)
{
    int sz;

    switch (cap_get_capType(cap)) {
    case cap_frame_cap: {
        clearMemory((void *)cap_frame_cap_get_capFBasePtr(cap), RISCV_4K_PageBits);
        Arch_finaliseCap(cap, is_final);
        return resetMemMapping(cap);
    }
    default:
        fail("Arch_recycleCap: Not implemented\n");
    }

    return cap_null_cap_new();
}

bool_t CONST
Arch_hasRecycleRights(cap_t cap)
{
    switch (cap_get_capType(cap)) {
    case cap_frame_cap:
        return cap_frame_cap_get_capFVMRights(cap) == VMReadWrite;

    default:
        return true;
    }
}


bool_t CONST
Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {
    case cap_frame_cap:
        if (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, topA, topB;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            topA = botA + MASK (pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            topB = botB + MASK (pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            return ((botA <= botB) && (topA >= topB) && (botB <= topB));
        }
        break;

    case cap_page_table_cap:
        if (cap_get_capType(cap_b) == cap_page_table_cap) {
            return cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        break;

    case cap_page_directory_cap:
        if (cap_get_capType(cap_b) == cap_page_directory_cap) {
            return cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        break;
    }

    return false;
}


bool_t CONST
Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    if ((cap_get_capType(cap_a) == cap_frame_cap) &&
            (cap_get_capType(cap_b) == cap_frame_cap)) {
        return ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                 cap_frame_cap_get_capFBasePtr(cap_b)) &&
                (cap_frame_cap_get_capFSize(cap_a) ==
                 cap_frame_cap_get_capFSize(cap_b)));
    }
    return Arch_sameRegionAs(cap_a, cap_b);
}

word_t
Arch_getObjectSize(word_t t)
{
    switch (t) {
    case seL4_RISCV_4K_Page:
    case seL4_RISCV_PageTableObject:
    case seL4_RISCV_LVL2PageTableObject:
    case seL4_RISCV_PageDirectoryObject:
        return RISCV_4K_PageBits;
    case seL4_RISCV_2M_Page:
        return RISCV_2M_PageBits;
    case seL4_RISCV_1G_Page:
        return RISCV_1G_PageBits;
    default:
        fail("Invalid object type");
        return 0;
    }
}

cap_t Arch_createObject(object_t t, void *regionBase, int userSize, bool_t
                        deviceMemory)
{
    switch (t) {
    case seL4_RISCV_4K_Page:
        if (!deviceMemory) {
            memzero(regionBase, 1 << RISCV_4K_PageBits);
        }

        return cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_4K_Page,                  /* capFSize             */
                   0,                              /* capFTMapype          */
                   0,                              /* capFMappedAddress    */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   0                               /* capFIsDevice         */
               );

    case seL4_RISCV_2M_Page: {
        if (!deviceMemory) {
            memzero(regionBase, 1 << RISCV_2M_PageBits);
        }

        return cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_2M_Page,                  /* capFSize             */
                   0,                              /* capFTMapype          */
                   0,                              /* capFMappedAddress    */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   0                               /* capFIsDevice         */
               );
    }

    case seL4_RISCV_1G_Page: {
        if (!deviceMemory) {
            memzero(regionBase, 1 << RISCV_1G_PageBits);
        }

        return cap_frame_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (word_t) regionBase,            /* capFBasePtr          */
                   RISCV_1G_Page,                  /* capFSize             */
                   0,                              /* capFTMapype          */
                   0,                              /* capFMappedAddress    */
                   wordFromVMRights(VMReadWrite),  /* capFVMRights         */
                   0                               /* capFIsDevice         */
               );
    }

    case seL4_RISCV_PageTableObject:
        memzero(regionBase, 1 << RISCV_4K_PageBits);

        return cap_page_table_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (word_t)regionBase,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    case seL4_RISCV_LVL2PageTableObject:
        memzero(regionBase, 1 << RISCV_4K_PageBits);

        return cap_lvl2_page_table_cap_new(
                   asidInvalid,            /* capLVL2PTMappedASID    */
                   (word_t)regionBase,     /* capLVL2PTBasePtr       */
                   0,                      /* capLVL2PTIsMapped      */
                   0                       /* capLVL2PTMappedAddress */
               );

    case seL4_RISCV_PageDirectoryObject:
        memzero(regionBase, 1 << RISCV_4K_PageBits);
        copyGlobalMappings((pde_t *)regionBase);

        return cap_page_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (word_t)regionBase,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    default:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t
Arch_decodeInvocation(
    word_t label,
    unsigned int length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    extra_caps_t extraCaps,
    bool_t call,
    word_t *buffer
)
{
    switch (cap_get_capType(cap)) {
    case cap_page_directory_cap:
    case cap_lvl2_page_table_cap:
    case cap_page_table_cap:
    case cap_frame_cap:
    case cap_asid_control_cap:
    case cap_asid_pool_cap:
        return decodeRISCVMMUInvocation(label, length, cptr, slot, cap, extraCaps, buffer);
    default:
        printf("Not page_cap");
    }
}

void
Arch_prepareThreadDelete(tcb_t *thread)
{
    /* No action required on RISCV. */
}

bool_t
Arch_isFrameType(word_t t)
{
    switch (t) {
    case seL4_RISCV_4K_Page:
        return true;
    default:
        return false;
    }
}
/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

