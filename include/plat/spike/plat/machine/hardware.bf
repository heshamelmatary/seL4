--
-- Copyright 2016, 2017 Hesham Almatary, Data61/CSIRO <hesham.almatary@data61.csiro.au>
-- Copyright 2015-2016 Hesham Almatary <heshamelmatary@gmail.com>
-- Author: Hesham Almatary <heshamelmatary@gmail.com>
--

--
-- Copyright 2014, General Dynamics C4 Systems
--
-- This software may be distributed and modified according to the terms of
-- the GNU General Public License version 2. Note that NO WARRANTY is provided.
-- See "LICENSE_GPLv2.txt" for details.
--
-- @TAG(GD_GPL)
--

base 32

block epitcr {
    padding 6
    field clksrc 2
    field om 2
    field stopen 1
    field dozen 1
    field waiten 1
    field dbgen 1
    field iovw 1
    field swr 1
    field prescaler 12
    field rld 1
    field ocien 1
    field enmod 1
    field en 1
}
