/**
 * lens_private.h
 *
 * Private header for the lens stepper driver interface.
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

#ifndef KERNEL_SRC_LENS_PRIVATE_H_
#define KERNEL_SRC_LENS_PRIVATE_H_

#include "lens.h"
#include "notifiers.h"
#include <linux/gpio.h>

enum {
        LENS_ENABLE,
        LENS_DECAY,
        LENS_FAULT,
        LENS_RESET,
        LENS_HOME,
        LENS_MODE0,
        LENS_MODE1,
        LENS_MODE2,
        LENS_NUM_GPIO
};

/** Microstepping modes */
enum lens_microstep_mode {
  MODE_FULL_STEP     = 1,
  MODE_MICROSTEPS_2  = 2,
  MODE_MICROSTEPS_4  = 4,
  MODE_MICROSTEPS_8  = 8,
  MODE_MICROSTEPS_16 = 16,
  MODE_MICROSTEPS_32 = 32,
};

/** Decay modes */
enum lens_decay_mode {
        MODE_DECAY_SLOW,
        MODE_DECAY_FAST
};

struct lens_gpio_config {
        const char *name;
        const u32 flags;  /* GPIOF_* flags from linux/gpio.h */
};

struct lens {
        /** Device that owns this data */
        struct device *dev;
        /** Lock to ensure mutually exclusive access */
        struct mutex lock;
        /** Bus Extender GPIO pins. */
        int lens_gpios[LENS_NUM_GPIO];
        /** Notifiers */
        struct notifier_block dms_notifier;
        /** State for GPIO Pins */
        /** 1=INPUT/OUTPUT HIGH, 0=OUTPUT LOW **/
        int gpio_state;
};

#endif
