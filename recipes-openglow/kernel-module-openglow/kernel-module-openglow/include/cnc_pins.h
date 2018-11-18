/**
 * cnc_pins.h
 *
 * Stepper driver GPIO pin mapping.
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

#ifndef KERNEL_SRC_CNC_PINS_H_
#define KERNEL_SRC_CNC_PINS_H_

#include "io.h"

enum {
        PIN_BEAM_DET_EN,
        PIN_FIRE,
        PIN_HV_OK,
        PIN_HV_WDOG,
        PIN_OK_2_FIRE,
        PIN_XY_ENABLE,
        PIN_X_STEP,
        PIN_X_DIR,
        PIN_X_DIAG0,
        PIN_X_DIAG1,
        PIN_X_LIMIT_P,
        PIN_X_LIMIT_N,
        PIN_Y1_STEP,
        PIN_Y1_DIR,
        PIN_Y1_DIAG0,
        PIN_Y1_DIAG1,
        PIN_Y1_LIMIT_P,
        PIN_Y1_LIMIT_N,
        PIN_Y2_STEP,
        PIN_Y2_DIR,
        PIN_Y2_DIAG0,
        PIN_Y2_DIAG1,
        PIN_Y2_LIMIT_P,
        PIN_Y2_LIMIT_N,
        PIN_Z_STEP,
        PIN_Z_DIR,
        NUM_GPIO_PINS
};

/**
 * Pin changes to apply at startup after successful initialization.
 */
extern const struct pin_change_set *cnc_startup_pin_changes;

/**
 * Pin changes to apply when stopping a cut normally.
 * Brings all drive lines low but keeps the steppers powered up.
 */
extern const struct pin_change_set *cnc_stop_pin_changes;

/**
 * Pin changes to apply when there is an emergency stop, or when unloading
 * the driver.
 * Brings all drive lines low and powers off the steppers.
 */
extern const struct pin_change_set *cnc_shutdown_pin_changes;

/**
 * Set indicating which step/dir pins are controlled by the SDMA script.
 */
extern const pin_set cnc_sdma_step_dir_pin_set;

/**
 * Set indicating which laser/aux pins are controlled by the SDMA script.
 */
extern const pin_set cnc_sdma_laser_aux_pin_set;

/**
 * Array of pin configurations.
 */
extern const struct pin_config pin_configs[NUM_GPIO_PINS];

#endif
