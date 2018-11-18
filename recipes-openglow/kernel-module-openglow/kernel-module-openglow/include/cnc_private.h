/**
 * cnc_private.h
 *
 * Private header for the stepper driver.
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

#ifndef KERNEL_SRC_CNC_PRIVATE_H_
#define KERNEL_SRC_CNC_PRIVATE_H_

#include "cnc.h"
#include "cnc_pins.h"
#include "openglow.h"

#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/kfifo.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/regulator/consumer.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/platform_data/dma-imx-sdma.h>
#include <linux/platform_data/epit-imx.h>

/* If 1, installs a handler that disables all hardware on a kernel panic. */
#define INSTALL_PANIC_HANDLER 0

struct cnc;

/**
 * Runs the SDMA script and begins processing queued pulse data.
 *
 * @return 0 on success
 *         -ENODATA if there is no pulse data enqueued,
 *         -EPERM if the driver is already running, or in the limit state.
 *
 * @note No operation is performed if the driver is already running.
 */
int cnc_run(struct cnc *self);

/**
 * Immediately stops the SDMA script and returns to the idle state.
 *
 * @return 0 on success
 *         -EPERM if the driver is in the limit state.
 */
int cnc_stop(struct cnc *self);

/**
 * Performs a controlled acceleration, running backward through `num_steps`
 * bytes of pulse data, then performs a controlled deceleration.
 *
 * @param num_steps Number of steps to run before decelerating
 * @return          0 on success
 *                  -EPERM if the driver is not in the IDLE state
 *                  -EINVAL if num_steps == 0
 *                  -ENODATA if the head is at the beginning of the data
 *
 * @note The pulse data buffer must contain at least `num_steps+N` bytes of
 *       valid data, where `N` is a constant determined by deceleration rate.
 *       If there is insufficient data enqueued, acceleration/deceleration stops
 *       when the oldest enqueued byte is reached.
 */
int cnc_backtrack(struct cnc *self, uint32_t num_steps);

/**
 * Performs a controlled acceleration with the laser disabled,
 * re-enabling the laser after `laser_delay_steps` bytes of pulse data have been
 * processed, and continues processing pulse data normally.
 *
 * @param laser_delay_steps Number of steps to run before re-enabling laser
 * @return                  0 on success
 *                          -EPERM if the driver is not in the IDLE state
 *                          -ENODATA if there is no pulse data enqueued
 *
 * @note If num_steps is 0, a controlled acceleration is performed, but the
 *       laser is never re-enabled.
 */
int cnc_resume(struct cnc *self, uint32_t laser_delay_steps);

/**
 * Transitions to the disabled state. Powers off the steppers.
 *
 * @return always 0
 */
int cnc_disable(struct cnc *self);

/**
 * Transitions the driver from the limit or disabled state to the idle state.
 *
 * @return 0 on success
 *         -EPERM if the driver is not in the idle, or disabled state.
 */
int cnc_enable(struct cnc *self);

/**
 * Clears LIMIT state to permit motion off of tripped limit switch.
 *
 * @return 0 on success
 *         -EPERM if the driver is not limit state.
 */
int cnc_ignore_limits(struct cnc *self);

/**
 * Pulses stepper once.
 *
 * @param direction Desired state of the direction line
 * @return          0 on success
 *                  -EPERM if the driver is not in the idle or disabled state
 */
int cnc_single_step(struct cnc *self, bool direction, int step_pin, int dir_pin);

/**
 * Pulses the X-axis stepper once.
 *
 * @param direction Desired state of the direction line
 * @return          0 on success
 *                  -EPERM if the driver is not in the idle or disabled state
 */
int cnc_single_x_step(struct cnc *self, bool direction);

/**
 * Pulses the Y-axis steppers (Y1, Y2) once.
 *
 * @param direction Desired state of the direction line
 * @return          0 on success
 *                  -EPERM if the driver is not in the idle or disabled state
 */
int cnc_single_y_step(struct cnc *self, bool direction);

/**
 * Pulses the Y1-axis stepper once.
 *
 * @param direction Desired state of the direction line
 * @return          0 on success
 *                  -EPERM if the driver is not in the idle or disabled state
 */
int cnc_single_y1_step(struct cnc *self, bool direction);

/**
 * Pulses the Y2-axis stepper once.
 *
 * @param direction Desired state of the direction line
 * @return          0 on success
 *                  -EPERM if the driver is not in the idle or disabled state
 */
int cnc_single_y2_step(struct cnc *self, bool direction);

/**
 * Pulses the Z-axis stepper once.
 *
 * @param direction Desired state of the direction line
 * @return          0 on success
 *                  -EPERM if the driver is not in the idle or disabled state
 */
int cnc_single_z_step(struct cnc *self, bool direction);

/**
 * Switches laser_on between output and hiz
 *
 * @return 0 on success
 */
int cnc_set_laser_output(struct cnc *self, int value);

/**
 * Clears all pulse data.
 *
 * @return 0 on success
 *         -EPERM if the driver is running
 */
int cnc_clear_pulse_data(struct cnc *self, enum cnc_lseek_options opts);

/**
 * Returns the current state.
 */
enum cnc_state cnc_state(struct cnc *self);

/**
 * Returns a string representation of the current state.
 */
const char *cnc_state_string(struct cnc *self);

/**
 * Returns a string representation of the given state.
 */
const char *cnc_string_for_state(enum cnc_state st);

/**
 * Retrieves the current position of the stepper driver and stores it in the
 * provided pos struct.
 */
int cnc_get_position(struct cnc *self, struct cnc_position *pos);

/**
 * Returns the step frequency in Hz.
 */
u32 cnc_get_step_frequency(struct cnc *self);

/**
 * Sets the step frequency in Hz.
 *
 * @param freq  Frequency in Hz
 * @return      0 on success
 *              -ERANGE if freq is not within the valid range
 *              -EBUSY if the value cannot be set (e.g. driver is running)
 */
int cnc_set_step_frequency(struct cnc *self, u32 freq);

/**
 * Called when the stepper driver's running state changes.
 */
void cnc_notify_state_changed(struct cnc *self);

/**
 * For debugging: dumps a human-readable printout of the SDMA context to
 * the supplied buffer, which must be at least PAGE_SIZE in length.
 *
 * @param buf   pointer to buffer to fill
 * @return      number of bytes written to buffer, or error code
 */
ssize_t cnc_print_sdma_context(struct cnc *self, char *buf);

/**
 * Allocates contiguous memory for the pulse data buffer.
 *
 * @return      0 on success, nonzero on error
 */
int cnc_buffer_init(struct cnc *self);

/**
 * Deallocates the pulse data buffer.
 */
void cnc_buffer_destroy(struct cnc *self);

/**
 * Enqueues a chunk of pulse data from userspace in the buffer.
 *
 * @param data  Pointer to userspace data buffer
 * @param count Number of bytes to enqueue
 * @return      On success, number of bytes enqueued; otherwise, < 0
 */
ssize_t cnc_buffer_add_user_data(struct cnc *self, const uint8_t __user *data,
  size_t count);

/**
 * Returns a bitmask indicating the size of the pulse data buffer.
 * Since the length of the pulse data buffer is a power of two, the returned
 * value is the buffer length minus 1.
 */
uint32_t cnc_buffer_fifo_bitmask(struct cnc *self);

/**
 * Returns the physical address of the start of the pulse data buffer.
 */
uint32_t cnc_buffer_fifo_start_phys(struct cnc *self);

/**
 * Returns nonzero if the pulse data buffer is empty.
 * May sleep; not safe to call from atomic context.
 */
int cnc_buffer_is_empty(struct cnc *self);

/**
 * Returns the total number of bytes enqueued in the pulse data buffer since
 * the last clear. Does not sleep.
 */
uint32_t cnc_buffer_total_bytes(struct cnc *self);

/**
 * Returns the maximum number of valid, contiguous data bytes that can be
 * backtracked over.
 */
uint32_t cnc_buffer_max_backtrack_length(struct cnc *self);

/**
 * Clears the given pulse data buffer attributes.
 * flags should be a bitwise-OR of one or more cnc_buffer_clear_flags.
 *
 * If CNC_BUFFER_CLEAR_DATA is set, all enqueued pulse data is dropped,
 * and the total byte count is reset to zero.
 *
 * If CNC_BUFFER_CLEAR_POSITION is set, only the X, Y, and Z step counters are
 * reset. The enqueued data is not touched.
 *
 * @param flags Flags indicating which attribute to clear (see below)
 * @return      0 on success, nonzero on error
 */
int cnc_buffer_clear(struct cnc *self, unsigned int flags);
/** Flags for cnc_buffer_clear(). */
enum cnc_buffer_clear_flag {
        CNC_BUFFER_CLEAR_DATA = 1,
        CNC_BUFFER_CLEAR_POSITION = 2
};

struct cnc_status {
        enum cnc_state state:8;
        int ignore_limits:1;
        int triggered_limits:8;
        int decelerating:1; /* 1 if currently decelerating */
        int accelerating:1; /* 1 if currently accelerating */
        int decel_on_interrupt:1; /* if 1, start decel on SDMA interrupt */
        int enable_laser_on_interrupt:1; /* if 1, enable laser on SDMA interrupt */
        int reserved:11;
} __attribute__((packed));

/**
 * Private data members.
 */
struct cnc {
        /** Device that owns this data */
        struct device *dev;
        /** /dev/openglow; device for receiving pulse data from userspace. */
        struct miscdevice pulsedev;
        /** Pointer to dirent of "state" sysfs attribute */
        struct kernfs_node *state_attr_node;
        /** Lock to ensure mutually exclusive access to /dev/openglow. */
        struct mutex lock;
        /** Hardware timer. */
        struct epit *epit;
        /** Pointer to the contiguous array of pulse data. */
        uint8_t *pulsebuf_virt; /* hi Jake! */
        /** Physical address of the contiguous array of pulse data. */
        dma_addr_t pulsebuf_phys;
        /** Size in bytes of the contiguous array of pulse data. */
        uint32_t pulsebuf_size;
        /** Total bytes of pulse data enqueued since last clear */
        uint32_t pulsebuf_total_bytes;
        /** FIFO data structure (uses contiguous array as backing buffer) */
        struct kfifo pulsebuf_fifo;
        /** Step frequency (in Hz) for the current job. */
        u32 step_freq;
        /**
        * If true, halt the driver if /dev/openglow is closed while in the
        * running state.
        */
        bool deadman_switch_active;
        /** SDMA script load address. (offset in bytes) */
        u32 sdma_script_origin;
        /** SDMA channel number. */
        u32 sdma_ch_num;
        /** Pointer to the SDMA engine. */
        struct sdma_engine *sdma;
        /** Pointer to the SDMA channel used by the driver. */
        struct sdma_channel *sdmac;
        /** The current state of the driver. */
        volatile struct cnc_status status;
        /**
        * Spinlock used to protect the state variable, which is shared between main
        * code (in kernel context) and the sdma channel and hrtimer callbacks (in
        * tasklet context). Must use spin_lock_bh()/spin_unlock_bh().
        */
        spinlock_t status_lock;
        /** GPIO pins. */
        int gpios[NUM_GPIO_PINS];
        /** Laser power PWM channel. */
        struct pwm_channel laser_pwm;
        /** 12V power supply. */
        struct regulator *supply_12v;
        /** 40V power supply. */
        struct regulator *supply_40v;
        /**
        * Atomic bitfield indicating limits that have not been handled.
        * If a limit needs to be asserted (esp. in hardirq context), use set_bit()
        * to set the appropriate bit in this field, and then schedule limit_tasklet.
        */
        volatile unsigned long pending_limits;
        /** Tasklet scheduled by limit interrupt handlers. */
        struct tasklet_struct limit_tasklet;
        /** Step frequency (in Hz) used during controlled accel/deceleration. */
        u32 ramp_step_freq;
        /**
        * Amount (in Hz) to reduce the step frequency at each step of controlled
        * acceleration/deceleration.
        */
        u32 ramp_step_freq_delta;
        /** Timer that drives acceleration/deceleration updates. */
        struct tasklet_hrtimer ramp_timer;
        /** Toggles the hv watchdog during a cut to keep the laser on. */
        struct hrtimer hv_wdog_timer;
#if INSTALL_PANIC_HANDLER
        /** Allows us to shut everything down if there's a panic */
        struct notifier_block panic_notifier;
#endif
};

/* defined in cnc_api.c */
extern const struct attribute_group cnc_attr_group;
extern const struct file_operations pulsedev_fops;

#endif
