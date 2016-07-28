--
-- Copyright 2014, General Dynamics C4 Systems
--
-- This software may be distributed and modified according to the terms of
-- the GNU General Public License version 2. Note that NO WARRANTY is provided.
-- See "LICENSE_GPLv2.txt" for details.
--
-- @TAG(GD_GPL)
--

#include <config.h>

---- Default base size: uint64_t
base 64(48,1)

-- Including the common structures.bf is neccessary because
-- we need the structures to be visible here when building
-- the capType
#include <object/structures_64.bf>

-- 4K frames
block frame_cap {
    field       capFMappedASID      16
    field_high  capFBasePtr         48

    field       capType             5
    field       capFSize            2
    field       capFMapType         2
    field_high  capFMappedAddress   48
    field       capFVMRights        2
    field       capFIsDevice        1
    padding                         4
}

-- Second-level page table
block page_table_cap {
    padding                         4
    field       capPTMappedASID     12
    field_high  capPTBasePtr        48

    field       capType             5
    padding                         10
    field       capPTIsMapped       1
    field_high  capPTMappedAddress  48
}

-- First-level page table (page directory)
block page_directory_cap {
    padding                         4
    field       capPDMappedASID     12
    field_high  capPDBasePtr        48

    field       capType             5
    padding                         10
    field       capPDIsMapped       1
    field_high  capPDMappedAddress  48
}

block lvl2_page_table_cap {
    padding                         4
    field       capLVL2PTMappedASID    12
    field_high  capLVL2PTBasePtr       48

    field       capType                 5
    padding                             10
    field       capLVL2PTIsMapped       1
    field_high  capLVL2PTMappedAddress  48
}

-- Cap to the table of 2^6 ASID pools
block asid_control_cap {
    padding 64

    field   capType     5   
    padding             59
}

-- Cap to a pool of 2^9 ASIDs
block asid_pool_cap {
    padding 64

    field       capType         5
    field       capASIDBase     12  
    padding                     11
    field_high  capASIDPool     36  
}

-- NB: odd numbers are arch caps (see isArchCap())
tagged_union cap capType {
    -- 5-bit tag caps
    tag null_cap            0
    tag untyped_cap         2
    tag endpoint_cap        4
    tag notification_cap    6
    tag reply_cap           8
    tag cnode_cap           10
    tag thread_cap          12
    tag irq_control_cap     14
    tag irq_handler_cap     16
    tag zombie_cap          18
    tag domain_cap	        20

    -- 5-bit tag arch caps
    tag frame_cap           1
    tag page_table_cap      3
    tag lvl2_page_table_cap 5
    tag page_directory_cap  7
    tag asid_control_cap    11  
    tag asid_pool_cap       13
}

---- Arch-independent object types


block vm_fault {
    -- TODO this is just padded to match non arch faults
    field     address           64

    padding                     32
    field     FSR               5
    padding                     7
    field     instructionFault  1
    padding                     16
    field     faultType         3
}

tagged_union fault faultType {
    tag null_fault 0
    tag cap_fault 1
    tag vm_fault 2
    tag unknown_syscall 3
    tag user_exception 4
}

-- VM attributes

block vm_attributes {
    padding 32
    padding 31
    field riscvExecuteNever  1
}

---- RISCV-specific object types

block stored_hw_asid {
    padding    32
    field asid 8
    field valid 1
    padding 21
    field pdeType 2
}

block pte {
    padding                16
    field physical_address 38
    field sw               3
    field dirty            1
    field read             1
    field type             4
    field valid            1
}

block pde {
    padding                   16
    field physical_address    38
    field sw                  3
    field dirty               1
    field read                1
    field type                4
    field valid               1
}
