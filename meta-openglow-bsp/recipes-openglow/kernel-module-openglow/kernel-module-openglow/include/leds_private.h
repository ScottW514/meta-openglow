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

#ifndef KERNEL_SRC_LEDS_PRIVATE_H_
#define KERNEL_SRC_LEDS_PRIVATE_H_

#include "leds.h"
#include "io.h"
#include "notifiers.h"

enum {
        PWM_LID_LED,
        LEDS_NUM_PWM_CHANNELS
};

enum {
        LED_RED,
        LED_GREEN,
        LED_BLUE,
        LEDS_NUM_LEDS
};

struct led_config {
        const char *name;
};

struct leds {
        /** Device that owns this data */
        struct device *dev;
        /** Lock to ensure mutually exclusive access */
        struct mutex lock;
        /** PWM channels. */
        struct pwm_channel pwms[LEDS_NUM_PWM_CHANNELS];
        /** LED Channels **/
        int leds[LEDS_NUM_LEDS];
        /** Notifiers */
        struct notifier_block dms_notifier;
};

#endif
