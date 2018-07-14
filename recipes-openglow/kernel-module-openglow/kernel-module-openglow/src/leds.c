/**
 * leds.c
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
#include "leds_private.h"
#include "device_attr.h"
#include "io.h"
#include "openglow.h"

#define SEL_LED(led, reg) (reg + led * 0x10)

extern struct kobject *openglow_kobj;

/** Module parameters */
extern int leds_enabled;

static const struct pwm_channel_config led_pwm_configs[LEDS_NUM_PWM_CHANNELS] = {
        [PWM_LID_LED]     = {"lid-led-pwm",    2000000}, /* 500 Hz */
};


static const struct led_config led_configs[LEDS_NUM_LEDS] = {
	[LED_RED]       = {"button-red-led"},
        [LED_GREEN]     = {"button-green-led"},
        [LED_BLUE]      = {"button-blue-led"},
};


static uint8_t leds_read_i2c_byte(struct i2c_client *client, int reg)
{
        struct leds *self = i2c_get_clientdata(client);
        uint8_t ret;
        mutex_lock(&self->lock);
        ret = i2c_smbus_read_byte_data(client, reg);
        mutex_unlock(&self->lock);
        return ret;
}


static int leds_write_i2c_byte(struct i2c_client *client, int reg, uint8_t new_value)
{
        struct leds *self = i2c_get_clientdata(client);
        int ret;
        mutex_lock(&self->lock);
        ret = i2c_smbus_write_byte_data(client, reg, new_value);
        mutex_unlock(&self->lock);
        return ret;
}


static void leds_make_safe(struct leds *self)
{
        int i;
        struct i2c_client *client = to_i2c_client(self->dev);
	i2c_smbus_write_byte_data(client, SEL_LED(LED_RED, REG_FAN_SETTING), 0x00);
        i2c_smbus_write_byte_data(client, SEL_LED(LED_GREEN, REG_FAN_SETTING), 0x00);
        i2c_smbus_write_byte_data(client, SEL_LED(LED_BLUE, REG_FAN_SETTING), 0x00);
	dev_err(self->dev, "making safe");
}


static int leds_dms_handler(struct notifier_block *nb, unsigned long action, void *data)
{
        struct leds *self = container_of(nb, struct leds, dms_notifier);
        leds_make_safe(self);
        return 0;
}


static ssize_t leds_read_register_ascii(struct device *dev, int reg, char *buf)
{
        struct i2c_client *client = to_i2c_client(dev);
        ssize_t value = leds_read_i2c_byte(client, reg);
        if (value >= 0) {
                return scnprintf(buf, PAGE_SIZE, "%hu\n", value);
        } else {
                return value;
        }
}


#define DEFINE_PWM_ATTR(name, pwm) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                struct leds *self = dev_get_drvdata(dev); \
                return scnprintf(buf, PAGE_SIZE, "%hu\n", io_pwm_get_duty_cycle(&self->pwms[pwm])); } \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct leds *self = dev_get_drvdata(dev); \
                unsigned long new_value; \
                ssize_t ret = kstrtoul(buf, 10, &new_value); \
                if (ret) { return ret; } \
                if (new_value > 100) { return -EINVAL; } \
                io_pwm_set_duty_cycle(&self->pwms[pwm], new_value); \
                return count; } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

#define DEFINE_LED_PWM_ATTR(name, reg) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                return leds_read_register_ascii(dev, reg, buf); } \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                struct i2c_client *client = to_i2c_client(dev); \
                unsigned long new_value; \
                ssize_t ret = kstrtoul(buf, 10, &new_value); \
                if (ret) { return ret; } \
                if (new_value > 100) { return -EINVAL; } \
                new_value = (255 * new_value) / 100; \
                ret = leds_write_i2c_byte(client, reg, new_value); \
                return (ret >= 0) ? count : ret; } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

DEFINE_PWM_ATTR(ATTR_LID_LED_PWM,  PWM_LID_LED);
DEFINE_LED_PWM_ATTR(ATTR_LED_RED_PWM,   SEL_LED(LED_RED, REG_FAN_SETTING));
DEFINE_LED_PWM_ATTR(ATTR_LED_GREEN_PWM, SEL_LED(LED_GREEN, REG_FAN_SETTING));
DEFINE_LED_PWM_ATTR(ATTR_LED_BLUE_PWM,  SEL_LED(LED_BLUE, REG_FAN_SETTING));

static struct attribute *leds_attrs[] = {
	DEV_ATTR_PTR(ATTR_LID_LED_PWM),
	DEV_ATTR_PTR(ATTR_LED_RED_PWM),
	DEV_ATTR_PTR(ATTR_LED_GREEN_PWM),
	DEV_ATTR_PTR(ATTR_LED_BLUE_PWM),
        NULL
};

const struct attribute_group leds_attr_group = {
        .attrs = leds_attrs
};

int leds_init_leds(struct device_node *of_node, const struct led_config *led_configs, int *leds, size_t nleds)
{
	int i;
        for (i = 0; i < nleds; i++) {
                const struct led_config *config = &led_configs[i];
                const char *name = config->name;
                if (unlikely(of_property_read_u32(of_node, name, &leds[i]) != 0)) {
                        pr_err("no definition found for \"%s\"\n", name);
			return -1;
                }
        }
        return 0;
}

int leds_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        struct leds *self;
        int ret = 0;
        if (!leds_enabled) { dev_info(&client->dev, "%s: disabled, skipping", __func__); return 0; }
        dev_info(&client->dev, "%s: started", __func__);

        /* Allocate driver data */
        self = devm_kzalloc(&client->dev, sizeof(*self), GFP_KERNEL);
        if (!self) {
                return -ENOMEM;
        }
        self->dev = &client->dev;
	mutex_init(&self->lock);
        i2c_set_clientdata(client, self);

        /* Set up PWMs */
        ret = io_init_pwms(client->dev.of_node, led_pwm_configs, self->pwms, LEDS_NUM_PWM_CHANNELS);
        if (ret) {
                goto failed_pwm_init;
        }

	/* Set up LEDs */
        ret = leds_init_leds(client->dev.of_node, led_configs, self->leds, LEDS_NUM_LEDS);
        if (ret) {
                goto failed_leds_init;
        }
	int manufacturer = i2c_smbus_read_byte_data(client, REG_MANUFACTURER_ID);
	int product_id = i2c_smbus_read_byte_data(client, REG_PRODUCT_ID);
	if (product_id != 0x35 || manufacturer != 0x5D) {
		dev_err(&client->dev, "led controller not detected\n");
                ret = -1;
                goto failed_leds_init;
	}
	/* Set all led outputs to 0 */
	ret = i2c_smbus_write_byte_data(client, SEL_LED(LED_RED, REG_FAN_SETTING), 0x00);
	ret += i2c_smbus_write_byte_data(client, SEL_LED(LED_GREEN, REG_FAN_SETTING), 0x00);
	ret += i2c_smbus_write_byte_data(client, SEL_LED(LED_BLUE, REG_FAN_SETTING), 0x00);
        /* PWM Divider to 52.  26KHz / 52 = 500 Hz */
        ret += i2c_smbus_write_byte_data(client, SEL_LED(LED_RED, REG_PWM_DIVIDE), 0x34);
	ret += i2c_smbus_write_byte_data(client, SEL_LED(LED_GREEN, REG_PWM_DIVIDE), 0x34);
	ret += i2c_smbus_write_byte_data(client, SEL_LED(LED_BLUE, REG_PWM_DIVIDE), 0x34);
        /* PWM Outputs to Push-Pull */
	ret += i2c_smbus_write_byte_data(client, REG_PWM_OUTPUT_CONFIG, 0x07);
	if (ret < 0) {
                dev_err(&client->dev, "failed to configure led controller");
                goto failed_leds_init;
        }



        /* Create sysfs attributes */
        ret = sysfs_create_group(&client->dev.kobj, &leds_attr_group);
        if (ret < 0) {
                dev_err(&client->dev, "failed to register attribute group");
                goto failed_create_group;
        }

        /* Add a link in /sys/openglow */
        ret = sysfs_create_link(openglow_kobj, &client->dev.kobj, LEDS_GROUP_NAME);
        if (ret) {
                goto failed_create_link;
        }

        /* Add deadman switch notifier */
        self->dms_notifier.notifier_call = leds_dms_handler;
        dms_notifier_chain_register(&dms_notifier_list, &self->dms_notifier);

        dev_info(&client->dev, "%s: done", __func__);
        return 0;

failed_create_link:
        sysfs_remove_group(&client->dev.kobj, &leds_attr_group);
failed_create_group:
        io_release_pwms(self->pwms, LEDS_NUM_PWM_CHANNELS);
failed_leds_init:
failed_pwm_init:
        return ret;
}


int leds_remove(struct i2c_client *client)
{
        struct leds *self = i2c_get_clientdata(client);
        if (!leds_enabled) { return 0; }
        dev_info(&client->dev, "%s: started", __func__);
        leds_make_safe(self);
        dms_notifier_chain_unregister(&dms_notifier_list, &self->dms_notifier);
        sysfs_remove_link(&client->dev.kobj, LEDS_GROUP_NAME);
        sysfs_remove_group(&client->dev.kobj, &leds_attr_group);
        io_release_pwms(self->pwms, LEDS_NUM_PWM_CHANNELS);
	mutex_destroy(&self->lock);
        dev_info(&client->dev, "%s: done", __func__);
        return 0;
}
