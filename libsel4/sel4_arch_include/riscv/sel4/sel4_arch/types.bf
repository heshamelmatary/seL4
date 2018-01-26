--
-- Copyright 2016, Data61
-- Commonwealth Scientific and Industrial Research Organisation (CSIRO)
-- ABN 41 687 119 230
--
-- This software may be distributed and modified according to the terms of
-- the BSD 2-Clause license. Note that NO WARRANTY is provided.
-- See "LICENSE_BSD2.txt" for details.
--
-- @TAG(D61_BSD)
--

#include <autoconf.h>

#if __riscv_xlen == 32
base 32

---- Arch-independent object types

block VMFault {
    padding   288
    field     IP                32
    field     Addr              32
    field     FSR               5
    padding                     8
    padding                     16
    field     seL4_FaultType    3
}

-- VM attributes
block vm_attributes {
    padding 31
    field riscvExecuteNever  1
}

block NullFault {
   padding 352
   padding 29
   field seL4_FaultType 3
}

block CapFault {
   padding 128
   field IP   32
   field Addr 32
   field InRecvPhase 32
   field LookupFailureType 32
   -- these vary according to LookupFailureType
   field MR4 32
   field MR5 32
   field MR6 32
   padding 29
   field seL4_FaultType 3
}

block UnknownSyscall {
   field FaultIP 32
   field SP 32
   field RA 32
   field A0 32
   field A1 32
   field A2 32
   field A3 32
   field A4 32
   field A5 32
   field A6 32
   field Syscall 32
   padding 29
   field seL4_FaultType 3
}

block UserException {
   padding 192
   field FaultIP 32
   field SP      32
   field FLAGS   32
   field Number  32
   field Code    32
   padding 29
   field seL4_FaultType 3
}

#else
base 64

---- Arch-independent object types

block VMFault {
    padding   576
    field     IP                64
    field     Addr              64
    padding                     32
    field     FSR               5
    padding                     8
    padding                     16
    field     seL4_FaultType    3
}

-- VM attributes
block vm_attributes {
    padding 32
    padding 31
    field riscvExecuteNever  1
}

block NullFault {
   padding 704
   padding 61
   field seL4_FaultType 3
}

block CapFault {
   padding 256
   field IP   64
   field Addr 64
   field InRecvPhase 64
   field LookupFailureType 64
   -- these vary according to LookupFailureType
   field MR4 64
   field MR5 64
   field MR6 64
   padding 61
   field seL4_FaultType 3
}

block UnknownSyscall {
   field FaultIP 64
   field SP 64
   field RA 64
   field A0 64
   field A1 64
   field A2 64
   field A3 64
   field A4 64
   field A5 64
   field A6 64
   field Syscall 64
   padding 61
   field seL4_FaultType 3
}

block UserException {
   padding 384
   field FaultIP 64
   field SP      64
   field FLAGS   64
   field Number  64
   field Code    64
   padding 61
   field seL4_FaultType 3
}
#endif

#include <sel4/arch/shared_types.bf>
