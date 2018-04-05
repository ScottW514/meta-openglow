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
#define PIC_GROUP_NAME            "pic"
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

#define ATTR_PIC_ID               id
#define ATTR_PIC_VERSION          version
#define ATTR_WATER_TEMP_1         water_temp_1
#define ATTR_WATER_TEMP_2         water_temp_2
#define ATTR_TEC_TEMP             tec_temp
#define ATTR_PWR_TEMP             pwr_temp
#define ATTR_LID_IR_1             lid_ir_1
#define ATTR_LID_IR_2             lid_ir_2
#define ATTR_LID_IR_3             lid_ir_3
#define ATTR_LID_IR_4             lid_ir_4
#define ATTR_HV_CURRENT           hv_current
#define ATTR_HV_VOLTAGE           hv_voltage
#define ATTR_X_CURRENT            x_step_current
#define ATTR_Y_CURRENT            y_step_current
#define ATTR_LID_LED              lid_led
#define ATTR_BUTTON_LED_1         button_led_1
#define ATTR_BUTTON_LED_2         button_led_2
#define ATTR_BUTTON_LED_3         button_led_3
#define ATTR_DAC1_ADC             dac1_adc
#define ATTR_DAC2_ADC             dac2_adc
#define ATTR_FVR_ADC              fvr_adc
#define ATTR_PIC_TEMP             pic_temp

#define ATTR_X_CURRENT_INIT       33
#define ATTR_Y_CURRENT_INIT       5


/**
 * Recommended buffer size for reading the value of the state attribute,
 * including a trailing null byte.
 */
#define STATE_BUF_LENGTH         16

/**
 * Expected value of PIC register 0xFF.
 */
#define PIC_MAGIC_NUMBER  0x4F47 /** ASCII 'OG' */

/**
 * Driver states.
 * Uses an X-macro that must be defined before use.
 */
#define DRIVER_STATES \
  X(STATE_IDLE, "idle")         /** Steppers are on but no cut is in progress */ \
  X(STATE_RUNNING, "running")   /** A cut is in progress */ \
  X(STATE_DISABLED, "disabled") /** Steppers were explicitly disabled (for debugging) */ \
  X(STATE_FAULT, "fault")     /** Stepper driver fault */

  /**
   * These match the register names used in the PIC source code,
   * which match the signal names on the schematic.
   * The sysfs attribute names are a little mor_XX##e human-readable.
   */
  enum pic_register
  {
    PIC_REG_ID            = 0xFF,
    PIC_REG_VERSION       = 0xFE,
    PIC_REG_WATER_THERM1  = 0x02,
    PIC_REG_WATER_THERM2  = 0x03,
    PIC_REG_TEC_THERM     = 0x04,
    PIC_REG_PWR_THERM     = 0x00,
    PIC_REG_LID_IR_DET1   = 0x08,
    PIC_REG_LID_IR_DET2   = 0x09,
    PIC_REG_LID_IR_DET3   = 0x0B,
    PIC_REG_LID_IR_DET4   = 0x0B,
    PIC_REG_STEP_DAC_X    = 0x21,
    PIC_REG_STEP_DAC_Y    = 0x22,
    PIC_REG_LID_LED       = 0x30,
    PIC_REG_BUTTON_LED1   = 0x31,
    PIC_REG_BUTTON_LED2   = 0x32,
    PIC_REG_BUTTON_LED3   = 0x33,
    PIC_REG_HV_ISENSE     = 0x12,
    PIC_REG_HV_VSENSE     = 0x13,
    PIC_REG_DAC1_ADC      = 0x1E,
    PIC_REG_DAC2_ADC      = 0x1C,
    PIC_REG_FVR_ADC       = 0x1F,
    PIC_REG_TEMP_ADC      = 0x1D
  };

  typedef uint8_t pic_command;
  typedef uint16_t pic_value;

#endif
