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
#define HEAD_FANS_GROUP_NAME        "head_fans"
#define LEDS_GROUP_NAME             "leds"
#define LENS_GROUP_NAME             "z-axis"
#define THERMAL_GROUP_NAME          "thermal"
#define TMC2130_GROUP_NAME          "tmc2130"

#define ROOT_CLASS_NAME             "class"

#define SYSFS_ROOT_DIR              "/sys/" ROOT_KOBJ_NAME "/"
#define SYSFS_ATTR_DIR              SYSFS_ROOT_DIR CNC_GROUP_NAME "/"
#define HEAD_FANS_SYSFS_ATTR_DIR    SYSFS_ROOT_DIR HEAD_FANS_GROUP_NAME "/"
#define LEDS_SYSFS_ATTR_DIR         SYSFS_ROOT_DIR LEDS_GROUP_NAME "/"
#define LENS_SYSFS_ATTR_DIR         SYSFS_ROOT_DIR LENS_GROUP_NAME "/"
#define THERMAL_SYSFS_ATTR_DIR      SYSFS_ROOT_DIR THERMAL_GROUP_NAME "/"
#define PULSE_DEVICE_DIR            "/dev/"
#define INPUT_DEVICE_DIR            "/dev/input/by-path/"

#define SYSFS_CLASS_DIR             "/sys/" ROOT_CLASS_NAME "/"

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

#define PULSE_DEVICE_PATH               PULSE_DEVICE_DIR PULSE_DEVICE_NAME
#define ATTR_STATE_PATH                 SYSFS_ATTR_DIR STR(ATTR_STATE)
#define ATTR_FAULTS_PATH                SYSFS_ATTR_DIR STR(ATTR_FAULTS)
#define ATTR_IGNORED_FAULTS_PATH        SYSFS_ATTR_DIR STR(ATTR_IGNORED_FAULTS)
#define ATTR_POSITION_PATH              SYSFS_ATTR_DIR STR(ATTR_POSITION)
#define ATTR_LASER_OUTPUT_PATH          SYSFS_ATTR_DIR STR(ATTR_LASER_OUTPUT)
#define ATTR_RUN_PATH                   SYSFS_ATTR_DIR STR(ATTR_RUN)
#define ATTR_STOP_PATH                  SYSFS_ATTR_DIR STR(ATTR_STOP)
#define ATTR_RESUME_PATH                SYSFS_ATTR_DIR STR(ATTR_RESUME)
#define ATTR_DISABLE_PATH               SYSFS_ATTR_DIR STR(ATTR_DISABLE)
#define ATTR_STEP_FREQ_PATH             SYSFS_ATTR_DIR STR(ATTR_STEP_FREQ)
#define ATTR_Z_STEP_PATH                SYSFS_ATTR_DIR STR(ATTR_Z_STEP)
#define ATTR_MOTOR_LOCK_PATH            SYSFS_ATTR_DIR STR(ATTR_MOTOR_LOCK)

#define ATTR_INTAKE_PWM             intake_pwm
#define ATTR_INTAKE1_TACH           intake_1_tach
#define ATTR_INTAKE2_TACH           intake_2_tach
#define ATTR_EXHAUST_PWM            exhaust_pwm
#define ATTR_EXHAUST_TACH           exhaust_tach
#define ATTR_WATER_HTR_PWM          water_heater_pwm
#define ATTR_WATER_PUMP_ON          water_pump_on
#define ATTR_TEC_ON                 tec_on

#define ATTR_INTAKE_PWM_PATH        THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE_PWM)
#define ATTR_INTAKE1_TACH_PATH      THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE1_TACH)
#define ATTR_INTAKE2_TACH_PATH      THERMAL_SYSFS_ATTR_DIR STR(ATTR_INTAKE2_TACH)
#define ATTR_EXHAUST_PWM_PATH       THERMAL_SYSFS_ATTR_DIR STR(ATTR_EXHAUST_PWM)
#define ATTR_EXHAUST_TACH_PATH      THERMAL_SYSFS_ATTR_DIR STR(ATTR_EXHAUST_TACH)
#define ATTR_WATER_HTR_PWM_PATH     THERMAL_SYSFS_ATTR_DIR STR(ATTR_WATER_HTR_PWM)
#define ATTR_WATER_PUMP_ON_PATH     THERMAL_SYSFS_ATTR_DIR STR(ATTR_WATER_PUMP_ON)
#define ATTR_TEC_ON_PATH            THERMAL_SYSFS_ATTR_DIR STR(ATTR_TEC_ON)

#define ATTR_AIR_ASSIST_PWM         air_assist_pwm
#define ATTR_AIR_ASSIST_TACH        air_assist_tach
#define ATTR_LENS_PURGE_PWM         lens_purge_pwm

#define ATTR_AIR_ASSIST_PWM_PATH    HEAD_FANS_SYSFS_ATTR_DIR STR(ATTR_AIR_ASSIST_PWM)
#define ATTR_AIR_ASSIST_TACH_PATH   HEAD_FANS_SYSFS_ATTR_DIR STR(ATTR_AIR_ASSIST_TACH)
#define ATTR_LENS_PURGE_PWM_PATH    HEAD_FANS_SYSFS_ATTR_DIR STR(ATTR_LENS_PURGE_PWM)

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
#define ATTR_STATUS                 status

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
#define ATTR_STATUS_PATH            TMC2130_SYSFS_ATTR_DIR STR(ATTR_STATUS)

#define ATTR_LID_LED_PWM            lid_led_pwm
#define ATTR_LED_RED_PWM            btn_red_led_pwm
#define ATTR_LED_GREEN_PWM          btn_green_led_pwm
#define ATTR_LED_BLUE_PWM           btn_blue_led_pwm

#define ATTR_LID_LED_PWM_PATH       LEDS_SYSFS_ATTR_DIR STR(ATTR_LID_LED_PWM)
#define ATTR_LED_RED_PWM_PATH       LEDS_SYSFS_ATTR_DIR STR(ATTR_LED_RED_PWM)
#define ATTR_LED_GREEN_PWM_PATH     LEDS_SYSFS_ATTR_DIR STR(ATTR_LED_GREEN_PWM)
#define ATTR_LED_BLUE_PWM_PATH      LEDS_SYSFS_ATTR_DIR STR(ATTR_LED_BLUE_PWM)

#define ATTR_LENS_ENABLE            enable
#define ATTR_LENS_DISABLE           disable
#define ATTR_LENS_STATUS            status
#define ATTR_LENS_FAULT             fault
#define ATTR_LENS_MODE              mode
#define ATTR_LENS_DECAY             decay
#define ATTR_LENS_HOME              home
#define ATTR_LENS_RESET             reset

#define ATTR_LENS_ENABLE_PATH       LENS_SYSFS_ATTR_DIR STR(ATTR_LENS_ENABLE)
#define ATTR_LENS_DISABLE_PATH      LENS_SYSFS_ATTR_DIR STR(ATTR_LENS_DISABLE)
#define ATTR_LENS_STATUS_PATH       LENS_SYSFS_ATTR_DIR STR(ATTR_LENS_STATUS)
#define ATTR_LENS_FAULT_PATH        LENS_SYSFS_ATTR_DIR STR(ATTR_LENS_FAULT)
#define ATTR_LENS_MODE_PATH         LENS_SYSFS_ATTR_DIR STR(ATTR_LENS_MODE)
#define ATTR_LENS_DECAY_PATH        LENS_SYSFS_ATTR_DIR STR(ATTR_LENS_DECAY)
#define ATTR_LENS_HOME_PATH         LENS_SYSFS_ATTR_DIR STR(ATTR_LENS_HOME)
#define ATTR_LENS_RESET_PATH        LENS_SYSFS_ATTR_DIR STR(ATTR_LENS_RESET)


/**
 * Recommended buffer size for reading the value of the state attribute,
 * including a trailing null byte.
 */
#define STATE_BUF_LENGTH            16

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

enum emc2303_global_registers
{
	REG_CONFIGURATION = 0x20,
	REG_FAN_STATUS = 0x24,
	REG_FAN_STALL_STATUS = 0x25,
	REG_FAN_SPIN_STATUS = 0x26,
	REG_DRIVE_FAIL_STATUS = 0x27,
	REG_FAN_INTERRUPT_ENABLE = 0x29,
	REG_PWM_POLARITY_CONFIG = 0x2a,
	REG_PWM_OUTPUT_CONFIG = 0x2b,
	REG_PWM_BASE_FREQ_1 = 0x2c,
	REG_PWM_BASE_FREQ_2 = 0x2d,
	REG_SOFTWARE_LOCK = 0xef,
	REG_PRODUCT_FEATURES = 0xfc,
	REG_PRODUCT_ID = 0xfd,
	REG_MANUFACTURER_ID = 0xfe,
	REG_REVISION = 0xff
};

enum emc2303_channel_registers
{
	REG_FAN_SETTING = 0x30,
	REG_PWM_DIVIDE = 0x31,
	REG_FAN_CONFIGURATION_1 = 0x32,
	REG_FAN_CONFIGURATION_2 = 0x33,
	REG_GAIN = 0x35,
	REG_FAN_SPIN_UP_CONFIG = 0x36,
	REG_FAN_MAX_STEP = 0x37,
	REG_FAN_MINIMUM_DRIVE = 0x38,
	REG_FAN_VALID_TACH_COUNT = 0x39,
	REG_FAN_DRIVE_FAIL_BAND_LOW = 0x3a,
	REG_FAN_DRIVE_FAIL_BAND_HIGH = 0x3b,
	REG_TACH_TARGET_LOW = 0x3c,
	REG_TACH_TARGET_HIGH = 0x3d,
	REG_TACH_READ_HIGH = 0x3e,
	REG_TACH_READ_LOW = 0x3f,
};

#endif
