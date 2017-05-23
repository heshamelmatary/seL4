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

#include <machine/assembler.h>

.global idle_thread
.section .text, "ax"

BEGIN_FUNC(idle_thread)
1:
    wfi
    j 1b
END_FUNC(idle_thread)
