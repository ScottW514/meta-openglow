/**
 * tmc2130_private.h
 *
 * Private header for Trinamic TMC2130 Driver.
 *
 * Copyright (C) 2018 Scott Wiederhold <s.e.wiederhold@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef KERNEL_SRC_TMC2130_PRIVATE_H_
#define KERNEL_SRC_TMC2130_PRIVATE_H_

#include "tmc2130.h"
#include "openglow.h"

/**
 * Private data members.
 */

 union {
         struct {
                 unsigned int reset_flag     :  1;
                 unsigned int driver_error   :  1;
                 unsigned int sg2            :  1;
                 unsigned int standstill     :  1;
         };
         unsigned long allfields;
 } SPI_STATUS;

 union {
         struct {
                 unsigned int reset          :  1;
                 unsigned int drv_err        :  1;
                 unsigned int uv_cp          :  1;
         };
         unsigned long allfields;
 } GSTAT;

 union {
         struct {
                 unsigned int STEP           :  1;
                 unsigned int DIR            :  1;
                 unsigned int DCEN_CFG4      :  1;
                 unsigned int DCIN_CFG5      :  1;
                 unsigned int DRV_ENN_CFG6   :  1;
                 unsigned int DCO            :  1;
                 unsigned int                : 18;
                 unsigned int VERSION        :  8;
         };
         unsigned long allfields;
 } IOIN;

 union {
         struct {
                 unsigned int SG_RESULT      : 10;
                 unsigned int                :  5;
                 unsigned int fsactive       :  1;
                 unsigned int CSACTUAL       :  5;
                 unsigned int                :  3;
                 unsigned int stallGuard     :  1;
                 unsigned int ot             :  1;
                 unsigned int otpw           :  1;
                 unsigned int s2ga           :  1;
                 unsigned int s2gb           :  1;
                 unsigned int ola            :  1;
                 unsigned int olb            :  1;
                 unsigned int stst           :  1;
         };
         unsigned long allfields;
 } DRV_STATUS;

 union {
         struct {
                 signed int cur_a            :  9;
                 unsigned int                :  7;
                 signed int cur_b            :  9;
         };
         unsigned long allfields;
 } MSCURACT;

 union {
         struct {
                 unsigned int b3             :  8;
                 unsigned int b2             :  8;
                 unsigned int b1             :  8;
                 unsigned int b0             :  8;
         };
         unsigned long allfields;
 } WORD32;

struct tmc2130 {
    struct device *dev;
    struct mutex lock;
    uint8_t status;
};

extern const struct attribute_group tmc2130_attr_group;

#endif
