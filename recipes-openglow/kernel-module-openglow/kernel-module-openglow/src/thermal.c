/**
 * thermal.c
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
#include "thermal_private.h"
#include "device_attr.h"
#include "io.h"
#include "openglow.h"
#include <linux/interrupt.h>

extern struct kobject *openglow_kobj;

/** Module parameters */
extern int thermal_enabled;

#pragma mark - GPIO/PWM definitions

static const struct pin_config thermal_pin_configs[THERMAL_NUM_GPIO_PINS] = {
        /* pump should be on at start */
        [PIN_WATER_PUMP]      = {"water-pump-gpio", GPIOF_OUT_INIT_HIGH},
        [PIN_TEC]             = {"tec-gpio",        GPIOF_OUT_INIT_LOW},
};

static const struct pwm_channel_config thermal_pwm_configs[THERMAL_NUM_PWM_CHANNELS] = {
        [PWM_WATER_HTR]     = {"water-heater-pwm",    10000000},
};

/**
 * Pin changes to apply when the driver is unloaded.
 */
DEFINE_PIN_CHANGE_SET(thermal_shutdown_pin_changes,
        {PIN_WATER_PUMP, 0},
        {PIN_TEC, 0},
);

#pragma mark - Deadman switch handler

/* Deadman switch trip notification */
static void thermal_make_safe(struct thermal *self)
{
        int i;
        io_change_pins(self->gpios, THERMAL_NUM_GPIO_PINS, thermal_shutdown_pin_changes);
        for (i = 0; i < THERMAL_NUM_PWM_CHANNELS; i++) {
                io_pwm_set_duty_cycle(&self->pwms[i], 0);
        }
        dev_err(self->dev, "making safe");
}


static int thermal_dms_handler(struct notifier_block *nb, unsigned long action, void *data)
{
        struct thermal *self = container_of(nb, struct thermal, dms_notifier);
        thermal_make_safe(self);
        return 0;
}

#pragma mark - Sysfs API

#define DEFINE_PWM_ATTR(name, pwm) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                struct thermal *self = dev_get_drvdata(dev); \
                return scnprintf(buf, PAGE_SIZE, "%hu\n", io_pwm_get_duty_cycle(&self->pwms[pwm])); } \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct thermal *self = dev_get_drvdata(dev); \
                unsigned long new_value; \
                ssize_t ret = kstrtoul(buf, 10, &new_value); \
                if (ret) { return ret; } \
                if (new_value > 100) { return -EINVAL; } \
                io_pwm_set_duty_cycle(&self->pwms[pwm], new_value); \
                return count; } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

#define DEFINE_GPIO_ATTR(name, pin) \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct thermal *self = dev_get_drvdata(dev); char ch; \
                if (count < 1) { return -EINVAL; } \
                ch = *buf; if (ch != '0' && ch != '1') { return -EINVAL; } \
                gpio_set_value(self->gpios[pin], ch == '1'); \
                return count; } \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                struct thermal *self = dev_get_drvdata(dev); \
                return scnprintf(buf, PAGE_SIZE, "%d\n", (gpio_get_value(self->gpios[pin]) != 0)); } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

DEFINE_PWM_ATTR(ATTR_WATER_HTR_PWM,  PWM_WATER_HTR);
DEFINE_GPIO_ATTR(ATTR_WATER_PUMP_ON, PIN_WATER_PUMP);
DEFINE_GPIO_ATTR(ATTR_TEC_ON,        PIN_TEC);

static struct attribute *thermal_attrs[] = {
        DEV_ATTR_PTR(ATTR_WATER_HTR_PWM),
        DEV_ATTR_PTR(ATTR_WATER_PUMP_ON),
        DEV_ATTR_PTR(ATTR_TEC_ON),
        NULL
};

const struct attribute_group thermal_attr_group = {
        .attrs = thermal_attrs
};


#pragma mark - Probe/remove

int thermal_probe(struct platform_device *pdev)
{
        struct thermal *self;
        int ret = 0;
        if (!thermal_enabled) { dev_info(&pdev->dev, "%s: disabled, skipping", __func__); return 0; }
        dev_info(&pdev->dev, "%s: started", __func__);

        /* Allocate driver data */
        self = devm_kzalloc(&pdev->dev, sizeof(*self), GFP_KERNEL);
        if (!self) {
                return -ENOMEM;
        }
        self->dev = &pdev->dev;
        platform_set_drvdata(pdev, self);

        /* Set up GPIOs */
        ret = io_init_gpios(pdev->dev.of_node, thermal_pin_configs, self->gpios, THERMAL_NUM_GPIO_PINS);
        if (ret) {
                goto failed_io_init;
        }

        /* Set up PWMs */
        ret = io_init_pwms(pdev->dev.of_node, thermal_pwm_configs, self->pwms, THERMAL_NUM_PWM_CHANNELS);
        if (ret) {
                goto failed_pwm_init;
        }

        /* Create sysfs attributes */
        ret = sysfs_create_group(&pdev->dev.kobj, &thermal_attr_group);
        if (ret < 0) {
                dev_err(&pdev->dev, "failed to register attribute group");
                goto failed_create_group;
        }

        /* Add a link in /sys/openglow */
        ret = sysfs_create_link(openglow_kobj, &pdev->dev.kobj, THERMAL_GROUP_NAME);
        if (ret) {
                goto failed_create_link;
        }

        /* Add deadman switch notifier */
        self->dms_notifier.notifier_call = thermal_dms_handler;
        dms_notifier_chain_register(&dms_notifier_list, &self->dms_notifier);

        dev_info(&pdev->dev, "%s: done", __func__);
        return 0;

failed_create_link:
        sysfs_remove_group(&pdev->dev.kobj, &thermal_attr_group);
failed_create_group:
        io_release_pwms(self->pwms, THERMAL_NUM_PWM_CHANNELS);
failed_pwm_init:
        io_release_gpios(self->gpios, THERMAL_NUM_GPIO_PINS);
failed_io_init:
        return ret;
}


int thermal_remove(struct platform_device *pdev)
{
        struct thermal *self = platform_get_drvdata(pdev);
        if (!thermal_enabled) { return 0; }
        dev_info(&pdev->dev, "%s: started", __func__);
        thermal_make_safe(self);
        dms_notifier_chain_unregister(&dms_notifier_list, &self->dms_notifier);
        sysfs_remove_link(&pdev->dev.kobj, THERMAL_GROUP_NAME);
        sysfs_remove_group(&pdev->dev.kobj, &thermal_attr_group);
        io_release_pwms(self->pwms, THERMAL_NUM_PWM_CHANNELS);
        io_release_gpios(self->gpios, THERMAL_NUM_GPIO_PINS);
        dev_info(&pdev->dev, "%s: done", __func__);
        return 0;
}
