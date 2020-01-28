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
        [PIN_BEAM_DET_EN]         = {"beam-det-enable-gpio", GPIOF_OUT_INIT_LOW}, /* informative signal for the head */
        [PIN_FIRE]                = {"laser-fire-gpio",      GPIOF_OUT_INIT_LOW},
        [PIN_HV_OK]               = {"hv-supply-ok-gpio",    GPIOF_IN},
        [PIN_HV_WDOG]             = {"hv-wdog-gpio",         GPIOF_OUT_INIT_LOW},
        [PIN_OK_2_FIRE]           = {"safe-to-fire-gpio",    GPIOF_IN},
        [PIN_XY_ENABLE]           = {"xy-enable-gpio",       GPIOF_OUT_INIT_HIGH}, /* active low */
        [PIN_X_STEP]              = {"x-step-gpio",          GPIOF_OUT_INIT_LOW},
        [PIN_X_DIR]               = {"x-dir-gpio",           GPIOF_OUT_INIT_LOW},
        [PIN_X_DIAG0]             = {"x-diag0-gpio",         GPIOF_IN},
        [PIN_X_DIAG1]             = {"x-diag1-gpio",         GPIOF_IN},
        [PIN_X_LIMIT_P]           = {"x-limit-p-gpio",       GPIOF_IN},
        [PIN_X_LIMIT_N]           = {"x-limit-n-gpio",       GPIOF_IN},
        [PIN_Y1_STEP]             = {"y1-step-gpio",         GPIOF_OUT_INIT_LOW},
        [PIN_Y1_DIR]              = {"y1-dir-gpio",          GPIOF_OUT_INIT_LOW},
        [PIN_Y1_DIAG0]            = {"y1-diag0-gpio",        GPIOF_IN},
        [PIN_Y1_DIAG1]            = {"y1-diag1-gpio",        GPIOF_IN},
        [PIN_Y1_LIMIT_P]          = {"y1-limit-p-gpio",      GPIOF_IN},
        [PIN_Y1_LIMIT_N]          = {"y1-limit-n-gpio",      GPIOF_IN},
        [PIN_Y2_STEP]             = {"y2-step-gpio",         GPIOF_OUT_INIT_LOW},
        [PIN_Y2_DIR]              = {"y2-dir-gpio",          GPIOF_OUT_INIT_LOW},
        [PIN_Y2_DIAG0]            = {"y2-diag0-gpio",        GPIOF_IN},
        [PIN_Y2_DIAG1]            = {"y2-diag1-gpio",        GPIOF_IN},
        [PIN_Y2_LIMIT_P]          = {"y2-limit-p-gpio",      GPIOF_IN},
        [PIN_Y2_LIMIT_N]          = {"y2-limit-n-gpio",      GPIOF_IN},
        [PIN_Z_STEP]              = {"z-step-gpio",          GPIOF_OUT_INIT_LOW},
        [PIN_Z_DIR]               = {"z-dir-gpio",           GPIOF_OUT_INIT_LOW},
};

const pin_set cnc_sdma_step_dir_pin_set =
        (1ULL << PIN_X_STEP)  |
        (1ULL << PIN_X_DIR)   |
        (1ULL << PIN_Y1_STEP) |
        (1ULL << PIN_Y1_DIR)  |
        (1ULL << PIN_Y2_STEP) |
        (1ULL << PIN_Y2_DIR)  |
        (1ULL << PIN_Z_STEP)  |
        (1ULL << PIN_Z_DIR);


const pin_set cnc_sdma_laser_aux_pin_set =
        (1ULL << PIN_FIRE)    |
        (1ULL << PIN_BEAM_DET_EN);


/** Pin changes to apply after initialization, or when reenabling the driver. */
DEFINE_PIN_CHANGE_SET(cnc_startup_pin_changes,
        {PIN_XY_ENABLE, 0}, /* active low */
        {PIN_FIRE, 0},
        {PIN_HV_WDOG, 0},
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
        {PIN_FIRE, 0},
        {PIN_HV_WDOG, 0},
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
        {PIN_FIRE, 0},
        {PIN_HV_WDOG, 0},
        {PIN_X_STEP, 0},
        {PIN_X_DIR, 0},
        {PIN_Y1_STEP, 0},
        {PIN_Y1_DIR, 0},
        {PIN_Y2_STEP, 0},
        {PIN_Y2_DIR, 0},
        {PIN_Z_STEP, 0},
        {PIN_Z_DIR, 0},
);
