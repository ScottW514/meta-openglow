/**
 * lens.c
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
#include <linux/delay.h>

#include "lens_private.h"
#include "device_attr.h"
#include "openglow.h"

extern struct kobject *openglow_kobj;

/** Module parameters */
extern int lens_enabled;

static const struct lens_gpio_config lens_gpio_configs[LENS_NUM_GPIO] = {
        [LENS_ENABLE]    = {"enable-gpio", GPIOF_OUT_INIT_HIGH | GPIOF_ACTIVE_LOW},
        [LENS_DECAY]     = {"decay-gpio",  GPIOF_OUT_INIT_LOW},
        [LENS_FAULT]     = {"fault-gpio",  GPIOF_IN | GPIOF_ACTIVE_LOW},
        [LENS_RESET]     = {"reset-gpio",  GPIOF_OUT_INIT_HIGH | GPIOF_ACTIVE_LOW},
        [LENS_HOME]      = {"home-gpio",   GPIOF_IN | GPIOF_ACTIVE_LOW},
        [LENS_MODE0]     = {"mode-0-gpio", GPIOF_OUT_INIT_LOW},
        [LENS_MODE1]     = {"mode-1-gpio", GPIOF_OUT_INIT_LOW},
        [LENS_MODE2]     = {"mode-2-gpio", GPIOF_OUT_INIT_LOW},
};


static int lens_disable(struct lens *self);


static void lens_make_safe(struct lens *self)
{
        int i;
        struct i2c_client *client = to_i2c_client(self->dev);
        lens_disable(self);
	dev_err(self->dev, "making safe");

}


static int lens_dms_handler(struct notifier_block *nb, unsigned long action, void *data)
{
        struct lens *self = container_of(nb, struct lens, dms_notifier);
        lens_make_safe(self);
        return 0;
}


static ssize_t lens_read_gpio(struct device *dev, int gpio)
{
        struct i2c_client *client = to_i2c_client(dev);
        struct lens *self = i2c_get_clientdata(client);
        const struct lens_gpio_config *config = &lens_gpio_configs[gpio];
        ssize_t ret = 0;
        ret = i2c_smbus_read_byte(client);
        if (ret > 0) {
                ret = (ret & (1 << self->lens_gpios[gpio])) > 0 ? 1 : 0;
                if (config->flags & GPIOF_ACTIVE_LOW) {
                        ret = ret ? 0 : 1;
                }
        }
        return ret;
}


static ssize_t lens_write_gpio(struct device *dev, int gpio, int value)
{
        struct i2c_client *client = to_i2c_client(dev);
        struct lens *self = i2c_get_clientdata(client);
        const struct lens_gpio_config *config = &lens_gpio_configs[gpio];
        ssize_t ret = 0;
        if (value > 1) {
                value = 1;
        }
        if (config->flags & GPIOF_ACTIVE_LOW) {
                value = value ? 0 : 1;
        }
        if (value == 1) {
                self->gpio_state |= (1 << self->lens_gpios[gpio]);
        } else {
                self->gpio_state &= ~(1 << self->lens_gpios[gpio]);
        }
	ret = i2c_smbus_write_byte(client, self->gpio_state);
        return ret;
}


static int lens_enable(struct lens *self)
{
        int ret = 0;
        mutex_lock(&self->lock);
        ret = lens_read_gpio(self->dev, LENS_ENABLE);
        if (ret == 1) {
                dev_err(self->dev, "%s: lens stepper already enabled", __func__);
                ret = -EINVAL;
        } else if (ret == 0) {
                dev_info(self->dev, "%s: enabling lens stepper", __func__);
                ret = lens_write_gpio(self->dev, LENS_ENABLE, 1);
        }
        mutex_unlock(&self->lock);
        return ret;
}


static int lens_disable(struct lens *self)
{
        int ret = 0;
        mutex_lock(&self->lock);
        ret = lens_read_gpio(self->dev, LENS_ENABLE);
        if (ret == 0) {
                dev_err(self->dev, "%s: lens stepper already disabled", __func__);
                ret = -EINVAL;
        } else if (ret == 1) {
                dev_info(self->dev, "%s: disabling lens stepper", __func__);
                ret = lens_write_gpio(self->dev, LENS_ENABLE, 0);
        }
        mutex_unlock(&self->lock);
        return ret;
}


static int lens_reset(struct lens *self)
{
        int ret = 0;
        dev_info(self->dev, "%s: resetting lens stepper", __func__);
        mutex_lock(&self->lock);
        ret = lens_write_gpio(self->dev, LENS_RESET, 1);
        udelay(5);
        ret += lens_write_gpio(self->dev, LENS_RESET, 0);
        mutex_unlock(&self->lock);
        return ret;
}


static ssize_t status_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        int ret = 0;
        if(lens_read_gpio(dev, LENS_FAULT) > 0) {
                return scnprintf(buf, PAGE_SIZE, "fault\n");
        }
        if(lens_read_gpio(dev, LENS_ENABLE) > 0) {
                return scnprintf(buf, PAGE_SIZE, "enabled\n");
        }
        return scnprintf(buf, PAGE_SIZE, "disabled\n");
}


static ssize_t fault_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        int ret = 0;
        ret = lens_read_gpio(dev, LENS_FAULT);
        if (ret < 0) return ret;
        return scnprintf(buf, PAGE_SIZE, "%d\n", ret);
}


static ssize_t home_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        int ret = 0;
        ret = lens_read_gpio(dev, LENS_HOME);
        if (ret < 0) return ret;
        return scnprintf(buf, PAGE_SIZE, "%d\n", ret);
}

static int lens_set_microstep_mode(struct lens *self, enum lens_microstep_mode mode)
{
        int mode_binary, pin_mode0, pin_mode1, pin_mode2, ret;
        switch (mode) {
                case MODE_FULL_STEP:     mode_binary = 0b000; break;
                case MODE_MICROSTEPS_2:  mode_binary = 0b001; break;
                case MODE_MICROSTEPS_4:  mode_binary = 0b010; break;
                case MODE_MICROSTEPS_8:  mode_binary = 0b011; break;
                case MODE_MICROSTEPS_16: mode_binary = 0b100; break;
                case MODE_MICROSTEPS_32: mode_binary = 0b101; break;
                default:                 return -EINVAL;
        }
        mutex_lock(&self->lock);
        ret = lens_write_gpio(self->dev, LENS_MODE0, mode_binary & 0b001);
        ret += lens_write_gpio(self->dev, LENS_MODE1, mode_binary & 0b010);
        ret += lens_write_gpio(self->dev, LENS_MODE2, mode_binary & 0b100);
        mutex_unlock(&self->lock);
        return ret;
}


static enum lens_microstep_mode lens_get_microstep_mode(struct lens *self)
{
        int mode_binary, pin_mode0, pin_mode1, pin_mode2, ret;
        mutex_lock(&self->lock);
        mode_binary = (lens_read_gpio(self->dev, LENS_MODE0) != 0) |
                ((lens_read_gpio(self->dev, LENS_MODE1) != 0) << 1) |
                ((lens_read_gpio(self->dev, LENS_MODE2) != 0) << 2);
        mutex_unlock(&self->lock);
        switch ((mode_binary) & 0b111) {
                case 0b000: return MODE_FULL_STEP;     break;
                case 0b001: return MODE_MICROSTEPS_2;  break;
                case 0b010: return MODE_MICROSTEPS_4;  break;
                case 0b011: return MODE_MICROSTEPS_8;  break;
                case 0b100: return MODE_MICROSTEPS_16; break;
                default:    return MODE_MICROSTEPS_32; break;
        }
}


static int lens_set_decay_mode(struct lens *self, enum lens_decay_mode mode)
{
        int pin;
        mutex_lock(&self->lock);
        switch (mode) {
                case MODE_DECAY_SLOW:  lens_write_gpio(self->dev, LENS_DECAY, 0); break;
                case MODE_DECAY_FAST:  lens_write_gpio(self->dev, LENS_DECAY, 1); break;
                default:               return -EINVAL;
        }
        mutex_unlock(&self->lock);
        return 0;
}


static enum lens_decay_mode lens_get_decay_mode(struct lens *self)
{
        return lens_read_gpio(self->dev, LENS_DECAY);
}


#define _DEFINE_COMMAND_ATTR(name) \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct lens *self = dev_get_drvdata(dev); char ch; int ret; \
                if (count < 1) { return -EINVAL; } \
                ch = *buf; if (ch != '1') { return -EINVAL; } \
                ret = lens_##name(self); \
                return (ret == 0) ? count : ret; \
        } \
        static DEVICE_ATTR(name, S_IWUSR, NULL, name##_store)
#define DEFINE_COMMAND_ATTR(name) _DEFINE_COMMAND_ATTR(name)

#define DEFINE_MODE_ATTR(name) \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct lens *self = dev_get_drvdata(dev); \
                int mode, ret; if (sscanf(buf, "%d", &mode) != 1) { return -EINVAL; } \
                ret = lens_set_microstep_mode(self, mode); \
                return (ret == 0) ? count : ret; } \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                struct lens *self = dev_get_drvdata(dev); \
                return scnprintf(buf, PAGE_SIZE, "%u\n", lens_get_microstep_mode(self)); } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

#define DEFINE_DECAY_ATTR(name) \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct lens *self = dev_get_drvdata(dev); \
                int mode, ret; if (sscanf(buf, "%d", &mode) != 1) { return -EINVAL; } \
                ret = lens_set_decay_mode(self, mode); \
                return (ret == 0) ? count : ret; } \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                struct lens *self = dev_get_drvdata(dev); \
                return scnprintf(buf, PAGE_SIZE, "%u\n", lens_get_decay_mode(self)); } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

DEFINE_COMMAND_ATTR(ATTR_LENS_ENABLE);
DEFINE_COMMAND_ATTR(ATTR_LENS_DISABLE);
DEFINE_DEVICE_ATTR(ATTR_LENS_STATUS, S_IRUSR, status_show, NULL);
DEFINE_DEVICE_ATTR(ATTR_LENS_FAULT, S_IRUSR, fault_show, NULL);
DEFINE_DEVICE_ATTR(ATTR_LENS_HOME, S_IRUSR, home_show, NULL);
DEFINE_MODE_ATTR(ATTR_LENS_MODE);
DEFINE_DECAY_ATTR(ATTR_LENS_DECAY);
DEFINE_COMMAND_ATTR(ATTR_LENS_RESET);

static struct attribute *lens_attrs[] = {
        DEV_ATTR_PTR(ATTR_LENS_ENABLE),
        DEV_ATTR_PTR(ATTR_LENS_DISABLE),
        DEV_ATTR_PTR(ATTR_LENS_STATUS),
        DEV_ATTR_PTR(ATTR_LENS_FAULT),
        DEV_ATTR_PTR(ATTR_LENS_HOME),
        DEV_ATTR_PTR(ATTR_LENS_MODE),
        DEV_ATTR_PTR(ATTR_LENS_DECAY),
        DEV_ATTR_PTR(ATTR_LENS_RESET),
        NULL
};


const struct attribute_group lens_attr_group = {
        .attrs = lens_attrs
};


int lens_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        struct lens *self;
        int ret = 0;
        if (!lens_enabled) { dev_info(&client->dev, "%s: disabled, skipping", __func__); return 0; }
        dev_info(&client->dev, "%s: started", __func__);

        /* Allocate driver data */
        self = devm_kzalloc(&client->dev, sizeof(*self), GFP_KERNEL);
        if (!self) {
                return -ENOMEM;
        }
        self->dev = &client->dev;
	mutex_init(&self->lock);
        i2c_set_clientdata(client, self);

        /* Set up GPIO's */
        int i;
        self->gpio_state = 0x00;
        for (i = 0; i < LENS_NUM_GPIO; i++) {
                const struct lens_gpio_config *config = &lens_gpio_configs[i];
                const char *name = config->name;
                if (unlikely(of_property_read_u32(client->dev.of_node, name, &self->lens_gpios[i]) != 0)) {
                        pr_err("no definition found for \"%s\"\n", name);
			ret = -1;
                        goto failed_gpio_init;
                }
                if ((config->flags & GPIOF_OUT_INIT_HIGH) | (config->flags & GPIOF_IN)) {
                        self->gpio_state |= (1 << self->lens_gpios[i]);
                }

        }
        ret = i2c_smbus_write_byte(client, self->gpio_state);
        if (ret) {
                dev_err(&client->dev, "failed to initialize bus extender");
                goto failed_gpio_init;
        }


        /* Create sysfs attributes */
        ret = sysfs_create_group(&client->dev.kobj, &lens_attr_group);
        if (ret < 0) {
                dev_err(&client->dev, "failed to register attribute group");
                goto failed_create_group;
        }

        /* Add a link in /sys/openglow */
        ret = sysfs_create_link(openglow_kobj, &client->dev.kobj, LENS_GROUP_NAME);
        if (ret) {
                goto failed_create_link;
        }

        /* Add deadman switch notifier */
        self->dms_notifier.notifier_call = lens_dms_handler;
        dms_notifier_chain_register(&dms_notifier_list, &self->dms_notifier);

        dev_info(&client->dev, "%s: done", __func__);
        return 0;

failed_create_link:
        sysfs_remove_group(&client->dev.kobj, &lens_attr_group);
failed_create_group:
failed_gpio_init:
        return ret;
}


int lens_remove(struct i2c_client *client)
{
        struct lens *self = i2c_get_clientdata(client);
        if (!lens_enabled) { return 0; }
        dev_info(&client->dev, "%s: started", __func__);
        lens_make_safe(self);
        dms_notifier_chain_unregister(&dms_notifier_list, &self->dms_notifier);
        sysfs_remove_link(&client->dev.kobj, LENS_GROUP_NAME);
        sysfs_remove_group(&client->dev.kobj, &lens_attr_group);
	mutex_destroy(&self->lock);
        dev_info(&client->dev, "%s: done", __func__);
        return 0;
}
