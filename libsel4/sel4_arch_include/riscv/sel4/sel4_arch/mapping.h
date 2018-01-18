/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */

#ifndef __LIBSEL4_SEL4_ARCH_MAPPING
#define __LIBSEL4_SEL4_ARCH_MAPPING

#include <autoconf.h>

#define SEL4_MAPPING_LOOKUP_LEVEL 2

#if CONFIG_PT_LEVELS == 3
#define SEL4_MAPPING_LOOKUP_NO_PT 21
#define SEL4_MAPPING_LOOKUP_NO_LVL2PT 30
#define SEL4_MAPPING_LOOKUP_NO_PD 39
#else
#error "These definitions need to be supported for the required PT levels"
#endif

static inline seL4_Word seL4_MappingFailedLookupLevel()
{
    return seL4_GetMR(SEL4_MAPPING_LOOKUP_LEVEL);
}

#endif
