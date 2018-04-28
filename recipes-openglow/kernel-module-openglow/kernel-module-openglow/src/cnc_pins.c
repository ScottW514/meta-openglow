/**
 * cnc_pins.c
 *
 * Copyright (C) 2018 Scott Wiederhold <s.e.wiederhold@gmail.com>
 * Portions Copyright (C) 2015-2018 Glowforge, Inc. <opensource@glowforge.com>
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
#include "cnc_pins.h"

/**
 * Names and flags for each GPIO pin.
 */
const struct pin_config pin_configs[NUM_GPIO_PINS] = {
        [PIN_XY_ENABLE]           = {"xy-enable-gpio",     GPIOF_OUT_INIT_HIGH}, /* active low */
        [PIN_X_STEP]              = {"x-step-gpio",        GPIOF_OUT_INIT_LOW},
        [PIN_X_DIR]               = {"x-dir-gpio",         GPIOF_OUT_INIT_LOW},
        [PIN_Y1_STEP]             = {"y1-step-gpio",       GPIOF_OUT_INIT_LOW},
        [PIN_Y1_DIR]              = {"y1-dir-gpio",        GPIOF_OUT_INIT_LOW},
        [PIN_Y2_STEP]             = {"y2-step-gpio",       GPIOF_OUT_INIT_LOW},
        [PIN_Y2_DIR]              = {"y2-dir-gpio",        GPIOF_OUT_INIT_LOW},
        [PIN_LASER_ON]            = {"laser-enable-gpio",  GPIOF_IN}, /* kept HiZ until explicitly enabled */
        [PIN_LASER_ON_HEAD]       = {"laser-on-head-gpio", GPIOF_OUT_INIT_LOW}, /* informative signal for the head */
        [PIN_CHARGE_PUMP]         = {"charge-pump-gpio",   GPIOF_OUT_INIT_LOW},
        [PIN_Z_STEP]              = {"z-step-gpio",        GPIOF_OUT_INIT_LOW},
        [PIN_Z_DIR]               = {"z-dir-gpio",         GPIOF_OUT_INIT_LOW},
};


const pin_set cnc_sdma_pin_set =
        (1ULL << PIN_X_STEP)  |
        (1ULL << PIN_X_DIR)   |
        (1ULL << PIN_Y1_STEP) |
        (1ULL << PIN_Y1_DIR)  |
        (1ULL << PIN_Y2_STEP) |
        (1ULL << PIN_Y2_DIR)  |
        (1ULL << PIN_Z_STEP)  |
        (1ULL << PIN_Z_DIR)   |
        (1ULL << PIN_LASER_ON)|
        (1ULL << PIN_LASER_ON_HEAD);


/** Pin changes to apply after initialization, or when reenabling the driver. */
DEFINE_PIN_CHANGE_SET(cnc_startup_pin_changes,
        {PIN_XY_ENABLE, 0}, /* active low */
        {PIN_CHARGE_PUMP, 0},
        {PIN_X_STEP, 0},
        {PIN_X_DIR, 0},
        {PIN_Y1_STEP, 0},
        {PIN_Y1_DIR, 0},
        {PIN_Y2_STEP, 0},
        {PIN_Y2_DIR, 0},
        {PIN_Z_STEP, 0},
        {PIN_Z_DIR, 0},
);


/**
 * Pin changes to apply when stopping a cut normally.
 * Brings all drive lines low but keeps the steppers powered up.
 */
DEFINE_PIN_CHANGE_SET(cnc_stop_pin_changes,
        {PIN_LASER_ON, HI_Z},
        {PIN_CHARGE_PUMP, 0},
        {PIN_X_STEP, 0},
        {PIN_X_DIR, 0},
        {PIN_Y1_STEP, 0},
        {PIN_Y1_DIR, 0},
        {PIN_Y2_STEP, 0},
        {PIN_Y2_DIR, 0},
        {PIN_Z_STEP, 0},
        {PIN_Z_DIR, 0},
);


/**
 * Pin changes to apply when the driver is disabled or the module is unloaded.
 * Brings all drive lines low and powers off the steppers.
 */
DEFINE_PIN_CHANGE_SET(cnc_shutdown_pin_changes,
        {PIN_XY_ENABLE, 1}, /* active low */
        {PIN_LASER_ON, HI_Z},
        {PIN_CHARGE_PUMP, 0},
        {PIN_X_STEP, 0},
        {PIN_X_DIR, 0},
        {PIN_Y1_STEP, 0},
        {PIN_Y1_DIR, 0},
        {PIN_Y2_STEP, 0},
        {PIN_Y2_DIR, 0},
        {PIN_Z_STEP, 0},
        {PIN_Z_DIR, 0},
);
