/*
 * Copyright 2018, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

/*
 * Copyright 2017, 2018 University of New South Wales, Australia
 * Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 */

#ifndef __ARCH_OBJECT_STRUCTURES_32_H
#define __ARCH_OBJECT_STRUCTURES_32_H

#include <config.h>

enum asidSizeConstants {
    asidHighBits = 5,
    asidLowBits = seL4_ASIDPoolIndexBits
};

#endif /* __ARCH_OBJECT_STRUCTURES_32_H */
