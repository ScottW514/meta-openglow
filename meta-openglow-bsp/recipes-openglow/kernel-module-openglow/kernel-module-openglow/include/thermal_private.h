/**
 * thermal_private.h
 *
 * Private header for the thermal interface.
 *
 * Copyright (C) 2018 Scott Wiederhold s.e.wiederhold@gmail.com>
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

#ifndef KERNEL_SRC_THERMAL_PRIVATE_H_
#define KERNEL_SRC_THERMAL_PRIVATE_H_

#include "thermal.h"
#include "io.h"
#include "notifiers.h"

enum {
        PIN_WATER_PUMP,
        PIN_TEC,
        THERMAL_NUM_GPIO_PINS
};

enum {
        PWM_WATER_HTR,
        THERMAL_NUM_PWM_CHANNELS
};

enum {
        FAN_EXHAUST,
        FAN_INTAKE1,
        FAN_INTAKE2,
        THERMAL_NUM_FANS
};

struct fan_config {
        const char *name;
};

struct thermal {
        /** Device that owns this data */
        struct device *dev;
        /** Lock to ensure mutually exclusive access */
        struct mutex lock;
        /** GPIO pins. */
        int gpios[THERMAL_NUM_GPIO_PINS];
        /** PWM channels. */
        struct pwm_channel pwms[THERMAL_NUM_PWM_CHANNELS];
        /** Fan Channels **/
        int fans[THERMAL_NUM_FANS];
        /** Notifiers */
        struct notifier_block dms_notifier;
};

#endif
