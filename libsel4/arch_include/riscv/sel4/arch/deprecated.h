/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#ifndef __LIBSEL4_ARCH_DEPRECATED_H
#define __LIBSEL4_ARCH_DEPRECATED_H

#include <autoconf.h>
#include <sel4/types.h>
#include <sel4/arch/syscalls.h>

#ifdef CONFIG_LIB_SEL4_HAVE_REGISTER_STUBS
static inline seL4_MessageInfo_t __attribute__((deprecated("Use seL4_ReplyRecvWithMRs")))
seL4_ReplyWaitWithMRs(seL4_CPtr src, seL4_MessageInfo_t msgInfo, seL4_Word *sender,
                      seL4_Word *mr0, seL4_Word *mr1, seL4_Word *mr2, seL4_Word *mr3)
{
    return seL4_ReplyRecvWithMRs(src, msgInfo, sender, mr0, mr1, mr2, mr3);
}

#endif /* CONFIG_LIB_SEL4_HAVE_REGISTER_STUBS */
#endif /* __ARCH_DEPRECATED_H__ */
/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

