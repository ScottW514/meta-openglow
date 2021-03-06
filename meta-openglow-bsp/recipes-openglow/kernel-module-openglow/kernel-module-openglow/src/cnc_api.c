/**
 * cnc_api.c
 *
 * Stepper driver userspace API handlers.
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

#include "cnc_private.h"
#include "device_attr.h"
#include "notifiers.h"
#include "io.h"
#include <linux/io.h>
#include <linux/interrupt.h>

/* miscdevice sets filp's private_data pointer to itself */
#define DEV_SELF_FROM_FILP(filp) \
        struct device *dev = ((struct miscdevice *)filp->private_data)->parent; \
        struct cnc *self = dev_get_drvdata(dev)

static int pulsedev_open(struct inode *inode, struct file *filp)
{
        DEV_SELF_FROM_FILP(filp);
        if (!mutex_trylock(&self->lock)) {
                dev_warn(dev, PULSE_DEVICE_PATH " is in use");
                return -EBUSY;
        } else {
                dev_info(dev, PULSE_DEVICE_PATH " opened");
        }
        return 0;
}


static int pulsedev_close(struct inode *inode, struct file *filp)
{
        DEV_SELF_FROM_FILP(filp);
        /* If the driver is running, and the deadman switch is active, */
        /* halt the driver */
        enum cnc_state st = cnc_state(self);
        dev_info(dev, PULSE_DEVICE_PATH " closed, driver state is '%s', dms is %s",
        cnc_string_for_state(st),
        (self->deadman_switch_active) ? "active" : "inactive");
        if (self->deadman_switch_active && st == STATE_RUNNING) {
                dev_warn(dev, PULSE_DEVICE_PATH " closed while locked and driver is running! Emergency stop.");
                cnc_disable(self);
                dms_notifier_call_chain(&dms_notifier_list, 0, NULL);
        }
        cnc_set_laser_output(self, 1); /* lock laser on close */
        mutex_unlock(&self->lock);
        return 0;
}



/** Currently, no data is returned when /dev/openglow is read. */
static ssize_t pulsedev_read(struct file *filp, char __user *data, size_t count, loff_t *offp)
{
        return 0;
}


static ssize_t pulsedev_write(struct file *filp, const char __user *data, size_t count, loff_t *offp)
{
        DEV_SELF_FROM_FILP(filp);
        return cnc_buffer_add_user_data(self, data, count);
}


static int pulsedev_fsync(struct file *filp, loff_t start, loff_t end, int datasync)
{
        return 0;
}


static int pulsedev_flock(struct file *filp, int cmd, struct file_lock *fl)
{
        DEV_SELF_FROM_FILP(filp);
        /* Arm or disarm the dead man's switch */
        self->deadman_switch_active = (fl->fl_type != F_UNLCK);
        dev_info(dev, "dms is %s", (self->deadman_switch_active) ? "active" : "inactive");
        return 0;
}


/**
 * Seeking to certain offsets performs the following:
 * 0: clear pulse data, byte counters, and position counters
 * 1: clear pulse data and byte counters only
 * 2: clear position counters only
 */
static loff_t pulsedev_llseek(struct file *filp, loff_t off, int whence)
{
        DEV_SELF_FROM_FILP(filp);
        if (off < 0 || off > PULSEDEV_LSEEK_MAX_VALID_OFFSET || whence != SEEK_SET) {
                return -EINVAL;
        }
        return cnc_clear_pulse_data(self, (enum cnc_lseek_options)off);
}


static ssize_t state_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        struct cnc *self = dev_get_drvdata(dev);
        return scnprintf(buf, PAGE_SIZE, "%s\n", cnc_state_string(self));
}


static ssize_t step_freq_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        struct cnc *self = dev_get_drvdata(dev);
        return scnprintf(buf, PAGE_SIZE, "%u\n", cnc_get_step_frequency(self));
}


static ssize_t step_freq_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        struct cnc *self = dev_get_drvdata(dev);
        unsigned long new_freq;
        int ret = kstrtoul(buf, 10, &new_freq);
        if (ret) { return ret; }
        ret = cnc_set_step_frequency(self, new_freq);
        return (ret == 0) ? count : ret;
}


static ssize_t position_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        struct cnc *self = dev_get_drvdata(dev);
        struct cnc_position *pos = (struct cnc_position *)buf;
        int ret = cnc_get_position(self, pos);
        return (ret == 0) ? sizeof(*pos) : ret;
}


static ssize_t sdma_context_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        struct cnc *self = dev_get_drvdata(dev);
        return cnc_print_sdma_context(self, buf);
}


void cnc_notify_state_changed(struct cnc *self)
{
        /* safe to call from atomic context */
        if (self->state_attr_node) {
                sysfs_notify_dirent(self->state_attr_node);
        }
}


#define _DEFINE_COMMAND_ATTR(name) \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct cnc *self = dev_get_drvdata(dev); char ch; int ret; \
                if (count < 1) { return -EINVAL; } \
                ch = *buf; if (ch != '1') { return -EINVAL; } \
                ret = cnc_##name(self); \
                return (ret == 0) ? count : ret; \
        } \
        static DEVICE_ATTR(name, S_IWUSR, NULL, name##_store)
#define DEFINE_COMMAND_ATTR(name) _DEFINE_COMMAND_ATTR(name)

#define DEFINE_BOOL_ATTR(name, fn) \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct cnc *self = dev_get_drvdata(dev); char ch; int ret; \
                if (count < 1) { return -EINVAL; } \
                ch = *buf; if (ch != '0' && ch != '1') { return -EINVAL; } \
                ret = fn(self, ch == '1'); \
                return (ret == 0) ? count : ret; \
        } \
        static DEVICE_ATTR(name, S_IWUSR, NULL, name##_store)

/* Negative values: accelerate backwards, then decelerate and stop */
/* Positive values: accelerate forward, reenable laser, and continue */
/* Zero: accelerate forward, continue without reenabling laser */
static ssize_t resume_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
        long long raw_value;
        int32_t value;
        /* For ease of interfacing, allow signed and unsigned 2s complement values. */
        /* (i.e. treat numbers in [2147483648,4294967295] as negative) */
        struct cnc *self = dev_get_drvdata(dev);
        int ret = kstrtoll(buf, 10, &raw_value);
        if (ret) { return ret; }
        value = raw_value;
        if (value < 0) {
                ret = cnc_backtrack(self, -value);
        } else {
                ret = cnc_resume(self, value);
        }
        return (ret == 0) ? count : ret;
}


DEFINE_BOOL_ATTR(ATTR_LASER_OUTPUT, cnc_set_laser_output);
DEFINE_BOOL_ATTR(ATTR_X_STEP,  cnc_single_x_step);
DEFINE_BOOL_ATTR(ATTR_Y_STEP,  cnc_single_y_step);
DEFINE_BOOL_ATTR(ATTR_Y1_STEP, cnc_single_y1_step);
DEFINE_BOOL_ATTR(ATTR_Y2_STEP, cnc_single_y2_step);
DEFINE_BOOL_ATTR(ATTR_Z_STEP,  cnc_single_z_step);
DEFINE_COMMAND_ATTR(ATTR_DISABLE);
DEFINE_COMMAND_ATTR(ATTR_ENABLE);
DEFINE_COMMAND_ATTR(ATTR_IGNORE_LIMITS);
DEFINE_COMMAND_ATTR(ATTR_RUN);
DEFINE_COMMAND_ATTR(ATTR_STOP);
DEFINE_DEVICE_ATTR(ATTR_POSITION, S_IRUSR, position_show, NULL);
DEFINE_DEVICE_ATTR(ATTR_RESUME, S_IWUSR, NULL, resume_store);
DEFINE_DEVICE_ATTR(ATTR_SDMA_CONTEXT, S_IRUSR, sdma_context_show, NULL);
DEFINE_DEVICE_ATTR(ATTR_STATE, S_IRUSR, state_show, NULL);
DEFINE_DEVICE_ATTR(ATTR_STEP_FREQ, S_IRUSR|S_IWUSR, step_freq_show, step_freq_store);

static struct attribute *cnc_attrs[] = {
        DEV_ATTR_PTR(ATTR_DISABLE),
        DEV_ATTR_PTR(ATTR_ENABLE),
        DEV_ATTR_PTR(ATTR_IGNORE_LIMITS),
        DEV_ATTR_PTR(ATTR_LASER_OUTPUT),
        DEV_ATTR_PTR(ATTR_POSITION),
        DEV_ATTR_PTR(ATTR_RESUME),
        DEV_ATTR_PTR(ATTR_RUN),
        DEV_ATTR_PTR(ATTR_SDMA_CONTEXT),
        DEV_ATTR_PTR(ATTR_STATE),
        DEV_ATTR_PTR(ATTR_STEP_FREQ),
        DEV_ATTR_PTR(ATTR_STOP),
        DEV_ATTR_PTR(ATTR_X_STEP),
        DEV_ATTR_PTR(ATTR_Y_STEP),
        DEV_ATTR_PTR(ATTR_Y1_STEP),
        DEV_ATTR_PTR(ATTR_Y2_STEP),
        DEV_ATTR_PTR(ATTR_Z_STEP),
        NULL
};

const struct attribute_group cnc_attr_group = {
        .attrs = cnc_attrs
};

const struct file_operations pulsedev_fops = {
        .owner    = THIS_MODULE,
        .open     = pulsedev_open,
        .read     = pulsedev_read,
        .write    = pulsedev_write,
        .fsync    = pulsedev_fsync,
        .flock    = pulsedev_flock,
        .llseek   = pulsedev_llseek,
        .release  = pulsedev_close,
};
