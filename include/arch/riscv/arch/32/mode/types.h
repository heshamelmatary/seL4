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

#ifndef __ARCH_MODE_TYPES_H
#define __ARCH_MODE_TYPES_H

#include <config.h>
#include <assert.h>
#include <stdint.h>

compile_assert(long_is_32bits, sizeof(unsigned long) == 4)

#define wordRadix 5

#endif
