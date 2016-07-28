--
-- Copyright 2014, General Dynamics C4 Systems
--
-- This software may be distributed and modified according to the terms of
-- the GNU General Public License version 2. Note that NO WARRANTY is provided.
-- See "LICENSE_GPLv2.txt" for details.
--
-- @TAG(GD_GPL)
--

base 64

block seL4_MessageInfo {
    padding 32
    field label 20
    field capsUnwrapped 3
    field extraCaps 2
    field length 7
}
