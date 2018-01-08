/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#ifndef __ARCH_TYPES_H
#define __ARCH_TYPES_H

#include <config.h>
#include <assert.h>
#include <stdint.h>

compile_assert(long_is_64bits, sizeof(unsigned long) == 8)

typedef unsigned long word_t;
typedef signed long  sword_t;
typedef word_t vptr_t;
typedef word_t paddr_t;
typedef word_t pptr_t;
typedef word_t cptr_t;
typedef word_t dev_id_t;
typedef word_t cpu_id_t;
typedef word_t node_id_t;
typedef word_t dom_t;

/* for libsel4 headers that the kernel shares */
typedef word_t seL4_Word;
typedef cptr_t seL4_CPtr;
typedef uint32_t seL4_Uint32;
typedef uint8_t seL4_Uint8;
typedef node_id_t seL4_NodeId;
typedef paddr_t seL4_PAddr;
typedef dom_t seL4_Domain;

#define wordRadix 6
#define wordBits (1 << wordRadix)

#endif
