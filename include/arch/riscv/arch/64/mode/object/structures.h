/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_OBJECT_STRUCTURES_64_H
#define __ARCH_OBJECT_STRUCTURES_64_H

#include <config.h>

enum asidSizeConstants {
    asidHighBits = 6,
    asidLowBits = seL4_ASIDPoolIndexBits
};

#define WORD_SIZE_BITS 3

#endif /* __ARCH_OBJECT_STRUCTURES_64_H */
