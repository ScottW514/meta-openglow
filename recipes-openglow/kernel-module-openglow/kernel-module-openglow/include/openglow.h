/**
 * openglow.h
 *
 * Userspace API to the OpenGlow kernel modules.
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

#define ROOT_KOBJ_NAME              "openglow"
#define CNC_GROUP_NAME              "cnc"
#define PIC_GROUP_NAME              "pic"
#define THERMAL_GROUP_NAME          "thermal"
#define TMC2130_GROUP_NAME          "tmc2130"

#define ROOT_CLASS_NAME             "class"

#define SYSFS_ROOT_DIR              "/sys/" ROOT_KOBJ_NAME "/"
#define SYSFS_ATTR_DIR              SYSFS_ROOT_DIR CNC_GROUP_NAME "/"
#define PIC_SYSFS_ATTR_DIR          SYSFS_ROOT_DIR PIC_GROUP_NAME "/"
#define THERMAL_SYSFS_ATTR_DIR      SYSFS_ROOT_DIR THERMAL_GROUP_NAME "/"
#define PULSE_DEVICE_DIR            "/dev/"
#define INPUT_DEVICE_DIR            "/dev/input/by-path/"

#define SYSFS_CLASS_DIR             "/sys/" ROOT_CLASS_NAME "/"
#define LED_SYSFS_ROOT              SYSFS_CLASS_DIR LED_GROUP_NAME "/"

#define PULSE_DEVICE_NAME           "openglow"

#define ATTR_STATE                  state
#define ATTR_FAULTS                 faults
#define ATTR_IGNORED_FAULTS         ignored_faults
#define ATTR_STEP_FREQ              step_freq
#define ATTR_RUN                    run
#define ATTR_STOP                   stop
#define ATTR_RESUME                 resume
#define ATTR_DISABLE                disable
#define ATTR_ENABLE                 enable
#define ATTR_Z_STEP                 z_step
#define ATTR_LASER_OUTPUT           laser_output
#define ATTR_MOTOR_LOCK             motor_lock
#define ATTR_POSITION               position
#define ATTR_SDMA_CONTEXT           sdma_context

#define ATTR_INTAKE1_TACH           tach_intake_1
#define ATTR_INTAKE2_TACH           tach_intake_2
#define ATTR_EXHAUST_TACH           tach_exhaust
#define ATTR_INTAKE_PWM             intake_pwm
#define ATTR_EXHAUST_PWM            exhaust_pwm
#define ATTR_HEATER_PWM             heater_pwm
#define ATTR_WATER_PUMP_ON          water_pump_on
#define ATTR_TEC_ON                 tec_on
#define ATTR_WATER_TEMP_1           water_temp_1
#define ATTR_WATER_TEMP_2           water_temp_2

#define PULSE_DEVICE_PATH           PULSE_DEVICE_DIR PULSE_DEVICE_NAME
#define ATTR_STATE_PATH             SYSFS_ATTR_DIR STR(ATTR_STATE)
#define ATTR_FAULTS_PATH            SYSFS_ATTR_DIR STR(ATTR_FAULTS)
#define ATTR_IGNORED_FAULTS_PATH    SYSFS_ATTR_DIR STR(ATTR_IGNORED_FAULTS)
#define ATTR_POSITION_PATH          SYSFS_ATTR_DIR STR(ATTR_POSITION)
#define ATTR_LASER_OUTPUT_PATH      SYSFS_ATTR_DIR STR(ATTR_LASER_OUTPUT)
#define ATTR_RUN_PATH               SYSFS_ATTR_DIR STR(ATTR_RUN)
#define ATTR_STOP_PATH              SYSFS_ATTR_DIR STR(ATTR_STOP)
#define ATTR_RESUME_PATH            SYSFS_ATTR_DIR STR(ATTR_RESUME)
#define ATTR_DISABLE_PATH           SYSFS_ATTR_DIR STR(ATTR_DISABLE)
#define ATTR_STEP_FREQ_PATH         SYSFS_ATTR_DIR STR(ATTR_STEP_FREQ)
#define ATTR_Z_STEP_PATH            SYSFS_ATTR_DIR STR(ATTR_Z_STEP)
#define ATTR_MOTOR_LOCK_PATH        SYSFS_ATTR_DIR STR(ATTR_MOTOR_LOCK)

#define ATTR_INTAKE_PWM_PATH        THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE_PWM)
#define ATTR_EXHAUST_PWM_PATH       THERMAL_SYSFS_ATTR_DIR STR(ATTR_EXHAUST_PWM)
#define ATTR_HEATER_PWM_PATH        THERMAL_SYSFS_ATTR_DIR STR(ATTR_HEATER_PWM)
#define ATTR_INTAKE1_TACH_PATH      THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE1_TACH)
#define ATTR_INTAKE2_TACH_PATH      THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE2_TACH)
#define ATTR_EXHAUST_TACH_PATH      THERMAL_SYSFS_ATTR_DIR STR(ATTR_EXHAUST_TACH)
#define ATTR_WATER_PUMP_ON_PATH     THERMAL_SYSFS_ATTR_DIR STR(ATTR_WATER_PUMP_ON)
#define ATTR_TEC_ON_PATH            THERMAL_SYSFS_ATTR_DIR STR(ATTR_TEC_ON)

#define ATTR_GCONF                  gconf
#define ATTR_GSTAT                  gstat
#define ATTR_IOIN                   ioin
#define ATTR_IHOLD_IRUN             ihold_irun
#define ATTR_TPOWERDOWN             tpowerdown
#define ATTR_TSTEP                  tstep
#define ATTR_TPWMTHRS               tpwmthrs
#define ATTR_TCOOLTHRS              tcoolthrs
#define ATTR_THIGH                  thigh
#define ATTR_XDIRECT                xdirect
#define ATTR_VDCMIN                 vdcmin
#define ATTR_MSLUT0                 mslut0
#define ATTR_MSLUT1                 mslut1
#define ATTR_MSLUT2                 mslut2
#define ATTR_MSLUT3                 mslut3
#define ATTR_MSLUT4                 mslut4
#define ATTR_MSLUT5                 mslut5
#define ATTR_MSLUT6                 mslut6
#define ATTR_MSLUT7                 mslut7
#define ATTR_MSLUTSEL               mslutsel
#define ATTR_MSLUTSTART             mslutstart
#define ATTR_MSCNT                  mscnt
#define ATTR_MSCURACT               mscuract
#define ATTR_CHOPCONF               chopconf
#define ATTR_COOLCONF               coolconf
#define ATTR_DCCTRL                 dcctrl
#define ATTR_DRV_STATUS             drv_status
#define ATTR_PWMCONF                pwmconf
#define ATTR_PWM_SCALE              pwm_scale
#define ATTR_ENCM_CTRL              encm_ctrl
#define ATTR_LOST_STEPS             lost_steps

#define ATTR_GCONF_PATH             TMC2130_SYSFS_ATTR_DIR STR(ATTR_GCONF)
#define ATTR_GSTAT_PATH             TMC2130_SYSFS_ATTR_DIR STR(ATTR_GSTAT)
#define ATTR_IOIN_PATH              TMC2130_SYSFS_ATTR_DIR STR(ATTR_IOIN)
#define ATTR_IHOLD_IRUN_PATH        TMC2130_SYSFS_ATTR_DIR STR(ATTR_IHOLD_IRUN)
#define ATTR_TPOWERDOWN_PATH        TMC2130_SYSFS_ATTR_DIR STR(ATTR_TPOWERDOWN)
#define ATTR_TSTEP_PATH             TMC2130_SYSFS_ATTR_DIR STR(ATTR_TSTEP)
#define ATTR_TPWMTHRS_PATH          TMC2130_SYSFS_ATTR_DIR STR(ATTR_TPWMTHRS)
#define ATTR_TCOOLTHRS_PATH         TMC2130_SYSFS_ATTR_DIR STR(ATTR_TCOOLTHRS)
#define ATTR_THIGH_PATH             TMC2130_SYSFS_ATTR_DIR STR(ATTR_THIGH)
#define ATTR_XDIRECT_PATH           TMC2130_SYSFS_ATTR_DIR STR(ATTR_XDIRECT)
#define ATTR_VDCMIN_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_VDCMIN)
#define ATTR_MSLUT0_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUT0)
#define ATTR_MSLUT1_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUT1)
#define ATTR_MSLUT2_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUT2)
#define ATTR_MSLUT3_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUT3)
#define ATTR_MSLUT4_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUT4)
#define ATTR_MSLUT5_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUT5)
#define ATTR_MSLUT6_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUT6)
#define ATTR_MSLUT7_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUT7)
#define ATTR_MSLUTSEL_PATH          TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUTSEL)
#define ATTR_MSLUTSTART_PATH        TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSLUTSTART)
#define ATTR_MSCNT_PATH             TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSCNT)
#define ATTR_MSCURACT_PATH          TMC2130_SYSFS_ATTR_DIR STR(ATTR_MSCURACT)
#define ATTR_CHOPCONF_PATH          TMC2130_SYSFS_ATTR_DIR STR(ATTR_CHOPCONF)
#define ATTR_COOLCONF_PATH          TMC2130_SYSFS_ATTR_DIR STR(ATTR_COOLCONF)
#define ATTR_DCCTRL_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_DCCTRL)
#define ATTR_DRV_STATUS_PATH        TMC2130_SYSFS_ATTR_DIR STR(ATTR_DRV_STATUS)
#define ATTR_PWMCONF_PATH           TMC2130_SYSFS_ATTR_DIR STR(ATTR_PWMCONF)
#define ATTR_PWM_SCALE_PATH         TMC2130_SYSFS_ATTR_DIR STR(ATTR_PWM_SCALE)
#define ATTR_ENCM_CTRL_PATH         TMC2130_SYSFS_ATTR_DIR STR(ATTR_ENCM_CTRL)
#define ATTR_LOST_STEPS_PATH        TMC2130_SYSFS_ATTR_DIR STR(ATTR_LOST_STEPS)


#define ATTR_PIC_ID                 id
#define ATTR_PIC_VERSION            version
#define ATTR_WATER_TEMP_1           water_temp_1
#define ATTR_WATER_TEMP_2           water_temp_2
#define ATTR_TEC_TEMP               tec_temp
#define ATTR_PWR_TEMP               pwr_temp
#define ATTR_LID_IR_1               lid_ir_1
#define ATTR_LID_IR_2               lid_ir_2
#define ATTR_LID_IR_3               lid_ir_3
#define ATTR_LID_IR_4               lid_ir_4
#define ATTR_HV_CURRENT             hv_current
#define ATTR_HV_VOLTAGE             hv_voltage
#define ATTR_X_CURRENT              x_step_current
#define ATTR_Y_CURRENT              y_step_current
#define ATTR_LID_LED                lid_led
#define ATTR_BUTTON_LED_1           button_led_1
#define ATTR_BUTTON_LED_2           button_led_2
#define ATTR_BUTTON_LED_3           button_led_3
#define ATTR_DAC1_ADC               dac1_adc
#define ATTR_DAC2_ADC               dac2_adc
#define ATTR_FVR_ADC                fvr_adc
#define ATTR_PIC_TEMP               pic_temp

#define ATTR_X_CURRENT_INIT         33
#define ATTR_Y_CURRENT_INIT         5

#define LED_TARGET_SUFFIX           "/target"
#define LED_SPEED_SUFFIX            "/speed"

#define BUTTON_LED_1_TARGET_FILE    LED_SYSFS_ROOT STR(ATTR_BUTTON_LED_1) LED_TARGET_SUFFIX
#define BUTTON_LED_2_TARGET_FILE    LED_SYSFS_ROOT STR(ATTR_BUTTON_LED_2) LED_TARGET_SUFFIX
#define BUTTON_LED_3_TARGET_FILE    LED_SYSFS_ROOT STR(ATTR_BUTTON_LED_3) LED_TARGET_SUFFIX
#define LID_LED_TARGET_FILE         LED_SYSFS_ROOT STR(ATTR_LID_LED) LED_TARGET_SUFFIX
#define LID_LED_SPEED_FILE          LED_SYSFS_ROOT STR(ATTR_LID_LED) LED_SPEED_SUFFIX


/**
 * Recommended buffer size for reading the value of the state attribute,
 * including a trailing null byte.
 */
#define STATE_BUF_LENGTH            16

/**
 * Expected value of PIC register 0xFF.
 */
#define PIC_MAGIC_NUMBER            0x4F47 /** ASCII 'OG' */


/**
 * Driver states.
 */
enum cnc_state {
  STATE_IDLE,
  STATE_RUNNING,
  STATE_DISABLED,
  STATE_FAULT,
  NUM_VALID_STATES,
  /**
   * An error occurred communicating with the kernel module,
   * or the status string is not one of the above
   */
  STATE_DRIVER_FAILURE
};


/** Axis constants */
enum cnc_axis {
  AXIS_X,
  AXIS_Y,
  NUM_AXES
};

/** Bits in the triggered_faults field */
enum fault_condition {
  FAULT_X,
  FAULT_Y1,
  FAULT_Y2,
  NUM_FAULT_CONDITIONS
};

/** Bits in the motor_lock field */
enum motor_lock_options {
  MOTOR_LOCK_X  = 1 << 0,
  MOTOR_LOCK_Y1 = 1 << 1,
  MOTOR_LOCK_Y2 = 1 << 2,
  MOTOR_LOCK_Z  = 1 << 3,
};

/** Struct returned by the `position` sysfs attribute */
struct cnc_position {
  int32_t x_step_pos;
  int32_t y_step_pos;
  int32_t z_step_pos;
  uint32_t bytes_processed;
  uint32_t bytes_total;
  int32_t reserved[3];
} __attribute__((packed));

/** Special lseek() offsets for /dev/glowforge */
enum cnc_lseek_options {
  PULSEDEV_LSEEK_CLEAR_DATA_AND_POSITION,
  PULSEDEV_LSEEK_CLEAR_DATA,
  PULSEDEV_LSEEK_CLEAR_POSITION,
  PULSEDEV_LSEEK_MAX_VALID_OFFSET = PULSEDEV_LSEEK_CLEAR_POSITION
};

enum tmc2130_register
{
    TMC2130_REG_GCONF           = 0x00,
    TMC2130_REG_GSTAT           = 0x01,
    TMC2130_REG_IOIN            = 0x04,
    TMC2130_REG_IHOLD_IRUN      = 0x10,
    TMC2130_REG_TPOWERDOWN      = 0x11,
    TMC2130_REG_TSTEP           = 0x12,
    TMC2130_REG_TPWMTHRS        = 0x13,
    TMC2130_REG_TCOOLTHRS       = 0x14,
    TMC2130_REG_THIGH           = 0x15,
    TMC2130_REG_XDIRECT         = 0x2D,
    TMC2130_REG_VDCMIN          = 0x33,
    TMC2130_REG_MSLUT0          = 0x60,
    TMC2130_REG_MSLUT1          = 0x61,
    TMC2130_REG_MSLUT2          = 0x62,
    TMC2130_REG_MSLUT3          = 0x63,
    TMC2130_REG_MSLUT4          = 0x64,
    TMC2130_REG_MSLUT5          = 0x65,
    TMC2130_REG_MSLUT6          = 0x66,
    TMC2130_REG_MSLUT7          = 0x67,
    TMC2130_REG_MSLUTSEL        = 0x68,
    TMC2130_REG_MSLUTSTART      = 0x69,
    TMC2130_REG_MSCNT           = 0x6A,
    TMC2130_REG_MSCURACT        = 0x6B,
    TMC2130_REG_CHOPCONF        = 0x6C,
    TMC2130_REG_COOLCONF        = 0x6D,
    TMC2130_REG_DCCTRL          = 0x6E,
    TMC2130_REG_DRV_STATUS      = 0x6F,
    TMC2130_REG_PWMCONF         = 0x70,
    TMC2130_REG_PWM_SCALE       = 0x71,
    TMC2130_REG_ENCM_CTRL       = 0x72,
    TMC2130_REG_LOST_STEPS      = 0x73
};

/**
 * These match the register names used in the PIC source code,
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
