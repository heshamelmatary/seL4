/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_API_TYPES
#define __ARCH_API_TYPES

#include <config.h>
#include <api/objecttype.h>
#include <arch/api/objecttype.h>


#define pageType PageObject4K

enum asidConstants {
    asidInvalid = 0
};

#define asidMax (BIT(asidLowBits + asidHighBits) - 1)

typedef uint32_t asid_t;

#endif
/*
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Author: Hesham Almatary <heshamelmatary@gmail.com>
 */

