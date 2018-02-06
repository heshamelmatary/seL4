/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_OBJECT_STRUCTURES_32_H
#define __ARCH_OBJECT_STRUCTURES_32_H

#include <config.h>

enum asidSizeConstants {
    asidHighBits = 5,
    asidLowBits = seL4_ASIDPoolIndexBits
};

#endif /* __ARCH_OBJECT_STRUCTURES_32_H */
