/**
 * openglow.h
 *
 * Userspace API to the Openglow kernel modules.
 *
 * Copyright (C) 2018 Scott Wiederhold <s.e.wiederhold@gmail.com>
 * Portions Copyright (C) 2015-2018 Glowforge, Inc. <opensource@glowforge.com>
 * Portions Written by Matt Sarnoff with contributions from Taylor Vaughn.
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

#ifndef KERNEL_UAPI_OPENGLOW_H_
#define KERNEL_UAPI_OPENGLOW_H_

#define STR2(x) #x
#define STR(x)  STR2(x)

#define ROOT_KOBJ_NAME            "openglow"
#define THERMAL_GROUP_NAME        "thermal"

#define ROOT_CLASS_NAME           "class"

#define SYSFS_ROOT_DIR            "/sys/" ROOT_KOBJ_NAME "/"
#define SYSFS_ATTR_DIR            SYSFS_ROOT_DIR CNC_GROUP_NAME "/"
#define PIC_SYSFS_ATTR_DIR        SYSFS_ROOT_DIR PIC_GROUP_NAME "/"
#define THERMAL_SYSFS_ATTR_DIR    SYSFS_ROOT_DIR THERMAL_GROUP_NAME "/"
#define PULSE_DEVICE_DIR          "/dev/"
#define INPUT_DEVICE_DIR          "/dev/input/by-path/"

#define SYSFS_CLASS_DIR           "/sys/" ROOT_CLASS_NAME "/"

#define ATTR_INTAKE1_TACH         tach_intake_1
#define ATTR_INTAKE2_TACH         tach_intake_2
#define ATTR_EXHAUST_TACH         tach_exhaust
#define ATTR_INTAKE_PWM           intake_pwm
#define ATTR_EXHAUST_PWM          exhaust_pwm
#define ATTR_HEATER_PWM           heater_pwm
#define ATTR_WATER_PUMP_ON        water_pump_on
#define ATTR_TEC_ON               tec_on
#define ATTR_WATER_TEMP_1         water_temp_1
#define ATTR_WATER_TEMP_2         water_temp_2

#define ATTR_INTAKE_PWM_PATH      THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE_PWM)
#define ATTR_EXHAUST_PWM_PATH     THERMAL_SYSFS_ATTR_DIR STR(ATTR_EXHAUST_PWM)
#define ATTR_HEATER_PWM_PATH      THERMAL_SYSFS_ATTR_DIR STR(ATTR_HEATER_PWM)
#define ATTR_INTAKE1_TACH_PATH    THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE1_TACH)
#define ATTR_INTAKE2_TACH_PATH    THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE2_TACH)
#define ATTR_EXHAUST_TACH_PATH    THERMAL_SYSFS_ATTR_DIR STR(ATTR_EXHAUST_TACH)
#define ATTR_WATER_PUMP_ON_PATH   THERMAL_SYSFS_ATTR_DIR STR(ATTR_WATER_PUMP_ON)
#define ATTR_TEC_ON_PATH          THERMAL_SYSFS_ATTR_DIR STR(ATTR_TEC_ON)


/**
 * Recommended buffer size for reading the value of the state attribute,
 * including a trailing null byte.
 */
#define STATE_BUF_LENGTH         16


/**
 * Driver states.
 * Uses an X-macro that must be defined before use.
 */
#define DRIVER_STATES \
  X(STATE_IDLE, "idle")         /** Steppers are on but no cut is in progress */ \
  X(STATE_RUNNING, "running")   /** A cut is in progress */ \
  X(STATE_DISABLED, "disabled") /** Steppers were explicitly disabled (for debugging) */ \
  X(STATE_FAULT, "fault")     /** Stepper driver fault */

#endif
