/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#include <arch/machine/registerset.h>

const register_t msgRegisters[] = {
    a2, a3, a4, a5
};

const register_t frameRegisters[] = {
    ra, sp, x3, x4, t0, t1, t2, s0, s1, a0, a1, a2, a3, a4, a5, a6, a7
};

const register_t gpRegisters[] = {

};

const register_t exceptionMessage[] = {
    SP /* TODO: Add other realted registers */
};

const register_t syscallMessage[] = {
    a0, a1, a2, a3, a4, a5, a5, a7
};
