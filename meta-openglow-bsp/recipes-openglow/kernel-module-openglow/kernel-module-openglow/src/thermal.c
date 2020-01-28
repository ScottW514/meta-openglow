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

#define SEL_FAN(fan, reg) (reg + fan * 0x10)

extern struct kobject *openglow_kobj;

/** Module parameters */
extern int thermal_enabled;


static const struct pin_config thermal_pin_configs[THERMAL_NUM_GPIO_PINS] = {
        /* pump should be on at start */
        [PIN_WATER_PUMP]      = {"water-pump-gpio", GPIOF_OUT_INIT_HIGH},
        [PIN_TEC]             = {"tec-gpio",        GPIOF_OUT_INIT_LOW},
};


static const struct pwm_channel_config thermal_pwm_configs[THERMAL_NUM_PWM_CHANNELS] = {
        [PWM_WATER_HTR]     = {"water-heater-pwm",    10000000},
};


static const struct fan_config fan_configs[THERMAL_NUM_FANS] = {
	[FAN_EXHAUST]     = {"exhaust-fan"},
	[FAN_INTAKE1]     = {"intake-fan-1"},
	[FAN_INTAKE2]     = {"intake-fan-2"},
};


/**
 * Pin changes to apply when the driver is unloaded.
 */
DEFINE_PIN_CHANGE_SET(thermal_shutdown_pin_changes,
        {PIN_WATER_PUMP, 0},
        {PIN_TEC, 0},
);


static uint8_t thermal_read_i2c_byte(struct i2c_client *client, int reg)
{
        struct thermal *self = i2c_get_clientdata(client);
        uint8_t ret;
        mutex_lock(&self->lock);
        ret = i2c_smbus_read_byte_data(client, reg);
        mutex_unlock(&self->lock);
        return ret;
}


static uint16_t thermal_read_i2c_word(struct i2c_client *client, int reg)
{
        struct thermal *self = i2c_get_clientdata(client);
        int ret;
        mutex_lock(&self->lock);
        ret = i2c_smbus_read_word_data(client, reg);
	ret = (ret >> 8) | (ret << 8);
        mutex_unlock(&self->lock);
        return ret;
}


static int thermal_write_i2c_byte(struct i2c_client *client, int reg, uint8_t new_value)
{
        struct thermal *self = i2c_get_clientdata(client);
        int ret;
        mutex_lock(&self->lock);
        ret = i2c_smbus_write_byte_data(client, reg, new_value);
        mutex_unlock(&self->lock);
        return ret;
}


static void thermal_make_safe(struct thermal *self)
{
        int i;
        io_change_pins(self->gpios, THERMAL_NUM_GPIO_PINS, thermal_shutdown_pin_changes);
        for (i = 0; i < THERMAL_NUM_PWM_CHANNELS; i++) {
                io_pwm_set_duty_cycle(&self->pwms[i], 0);
        }
        struct i2c_client *client = to_i2c_client(self->dev);
	i2c_smbus_write_byte_data(client, SEL_FAN(FAN_EXHAUST, REG_FAN_SETTING), 0x00);
	i2c_smbus_write_byte_data(client, SEL_FAN(FAN_INTAKE1, REG_FAN_SETTING), 0x00);
	i2c_smbus_write_byte_data(client, SEL_FAN(FAN_INTAKE2, REG_FAN_SETTING), 0x00);
	dev_err(self->dev, "making safe");
}


static int thermal_dms_handler(struct notifier_block *nb, unsigned long action, void *data)
{
        struct thermal *self = container_of(nb, struct thermal, dms_notifier);
        thermal_make_safe(self);
        return 0;
}


static ssize_t thermal_read_register_ascii(struct device *dev, int reg, char *buf)
{
        struct i2c_client *client = to_i2c_client(dev);
        ssize_t value = thermal_read_i2c_byte(client, reg);
        if (value >= 0) {
                return scnprintf(buf, PAGE_SIZE, "%hu\n", value);
        } else {
                return value;
        }
}


static ssize_t thermal_read_tach_ascii(struct device *dev, int reg, char *buf)
{
        struct i2c_client *client = to_i2c_client(dev);
        ssize_t value = thermal_read_i2c_word(client, reg) >> 3;
	int rpm = (3932160 * 2) / value;
	//  rpm = rpm > 999 ? rpm : 0;
        if (value >= 0) {
                return scnprintf(buf, PAGE_SIZE, "%hu\n", rpm);
        } else {
                return value;
        }
}


static ssize_t thermal_write_register_ascii(struct device *dev, int reg, const char *buf, size_t count)
{
        struct i2c_client *client = to_i2c_client(dev);
        uint8_t new_value = 0;
        int ret;
        /* Error if the entire buffer is used; we need a null-terminator */
        if (count >= PAGE_SIZE) { return -E2BIG; }

        if (sscanf(buf, "%hhu", &new_value) != 1) {
                return -EINVAL;
        }
        ret = thermal_write_i2c_byte(client, reg, new_value);
	return (ret >= 0) ? count : ret;
}


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

#define DEFINE_FAN_TACH_ATTR(name, reg) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                return thermal_read_tach_ascii(dev, reg, buf); } \
        static DEVICE_ATTR(name, S_IRUSR, name##_show, NULL)

#define DEFINE_FAN_PWM_ATTR(name, reg) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                return thermal_read_register_ascii(dev, reg, buf); } \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                return thermal_write_register_ascii(dev, reg, buf, count); } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

DEFINE_PWM_ATTR(ATTR_WATER_HTR_PWM,  PWM_WATER_HTR);
DEFINE_GPIO_ATTR(ATTR_WATER_PUMP_ON, PIN_WATER_PUMP);
DEFINE_GPIO_ATTR(ATTR_TEC_ON,        PIN_TEC);
DEFINE_FAN_PWM_ATTR(ATTR_EXHAUST_PWM,   SEL_FAN(FAN_EXHAUST, REG_FAN_SETTING));
DEFINE_FAN_PWM_ATTR(ATTR_INTAKE_PWM,    SEL_FAN(FAN_INTAKE1, REG_FAN_SETTING));
DEFINE_FAN_TACH_ATTR(ATTR_EXHAUST_TACH, SEL_FAN(FAN_EXHAUST, REG_TACH_READ_HIGH));
DEFINE_FAN_TACH_ATTR(ATTR_INTAKE1_TACH, SEL_FAN(FAN_INTAKE1, REG_TACH_READ_HIGH));
DEFINE_FAN_TACH_ATTR(ATTR_INTAKE2_TACH, SEL_FAN(FAN_INTAKE2, REG_TACH_READ_HIGH));

static struct attribute *thermal_attrs[] = {
        DEV_ATTR_PTR(ATTR_WATER_HTR_PWM),
        DEV_ATTR_PTR(ATTR_WATER_PUMP_ON),
	DEV_ATTR_PTR(ATTR_TEC_ON),
	DEV_ATTR_PTR(ATTR_EXHAUST_PWM),
	DEV_ATTR_PTR(ATTR_INTAKE_PWM),
	DEV_ATTR_PTR(ATTR_EXHAUST_TACH),
	DEV_ATTR_PTR(ATTR_INTAKE1_TACH),
	DEV_ATTR_PTR(ATTR_INTAKE2_TACH),
        NULL
};


const struct attribute_group thermal_attr_group = {
        .attrs = thermal_attrs
};


int thermal_init_fans(struct device_node *of_node, const struct fan_config *fan_configs, int *fans, size_t nfans)
{
	int i;
        for (i = 0; i < nfans; i++) {
                const struct fan_config *config = &fan_configs[i];
                const char *name = config->name;
                if (unlikely(of_property_read_u32(of_node, name, &fans[i]) != 0)) {
                        pr_err("no definition found for \"%s\"\n", name);
			return -1;
                }
        }
        return 0;
}


int thermal_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        struct thermal *self;
        int ret = 0;
        if (!thermal_enabled) { dev_info(&client->dev, "%s: disabled, skipping", __func__); return 0; }
        dev_info(&client->dev, "%s: started", __func__);

        /* Allocate driver data */
        self = devm_kzalloc(&client->dev, sizeof(*self), GFP_KERNEL);
        if (!self) {
                return -ENOMEM;
        }
        self->dev = &client->dev;
	mutex_init(&self->lock);
        i2c_set_clientdata(client, self);

        /* Set up GPIOs */
        ret = io_init_gpios(client->dev.of_node, thermal_pin_configs, self->gpios, THERMAL_NUM_GPIO_PINS);
        if (ret) {
                goto failed_io_init;
        }

        /* Set up PWMs */
        ret = io_init_pwms(client->dev.of_node, thermal_pwm_configs, self->pwms, THERMAL_NUM_PWM_CHANNELS);
        if (ret) {
                goto failed_pwm_init;
        }

	/* Set up FANs */
        ret = thermal_init_fans(client->dev.of_node, fan_configs, self->fans, THERMAL_NUM_FANS);
        if (ret) {
                goto failed_fan_init;
        }
	int manufacturer = i2c_smbus_read_byte_data(client, REG_MANUFACTURER_ID);
	int product_id = i2c_smbus_read_byte_data(client, REG_PRODUCT_ID);
	if (product_id != 0x35 || manufacturer != 0x5D) {
		dev_err(&client->dev, "fan controller not detected\n");
                ret = -1;
                goto failed_fan_init;
	}
	/* PWM Outputs to Push-Pull */
	ret = i2c_smbus_write_byte_data(client, REG_PWM_OUTPUT_CONFIG, 0x07);
	/* Set all fan outputs to 0 */
	ret += i2c_smbus_write_byte_data(client, SEL_FAN(FAN_EXHAUST, REG_FAN_SETTING), 0x00);
	ret += i2c_smbus_write_byte_data(client, SEL_FAN(FAN_INTAKE1, REG_FAN_SETTING), 0x00);
	ret += i2c_smbus_write_byte_data(client, SEL_FAN(FAN_INTAKE2, REG_FAN_SETTING), 0x00);
	if (ret < 0) {
                dev_err(&client->dev, "failed to configure fan controller");
                goto failed_fan_init;
        }



        /* Create sysfs attributes */
        ret = sysfs_create_group(&client->dev.kobj, &thermal_attr_group);
        if (ret < 0) {
                dev_err(&client->dev, "failed to register attribute group");
                goto failed_create_group;
        }

        /* Add a link in /sys/openglow */
        ret = sysfs_create_link(openglow_kobj, &client->dev.kobj, THERMAL_GROUP_NAME);
        if (ret) {
                goto failed_create_link;
        }

        /* Add deadman switch notifier */
        self->dms_notifier.notifier_call = thermal_dms_handler;
        dms_notifier_chain_register(&dms_notifier_list, &self->dms_notifier);

        dev_info(&client->dev, "%s: done", __func__);
        return 0;

failed_create_link:
        sysfs_remove_group(&client->dev.kobj, &thermal_attr_group);
failed_create_group:
        io_release_pwms(self->pwms, THERMAL_NUM_PWM_CHANNELS);
failed_fan_init:
failed_pwm_init:
        io_release_gpios(self->gpios, THERMAL_NUM_GPIO_PINS);
failed_io_init:
        return ret;
}


int thermal_remove(struct i2c_client *client)
{
        struct thermal *self = i2c_get_clientdata(client);
        if (!thermal_enabled) { return 0; }
        dev_info(&client->dev, "%s: started", __func__);
        thermal_make_safe(self);
        dms_notifier_chain_unregister(&dms_notifier_list, &self->dms_notifier);
        sysfs_remove_link(&client->dev.kobj, THERMAL_GROUP_NAME);
        sysfs_remove_group(&client->dev.kobj, &thermal_attr_group);
        io_release_pwms(self->pwms, THERMAL_NUM_PWM_CHANNELS);
        io_release_gpios(self->gpios, THERMAL_NUM_GPIO_PINS);
	mutex_destroy(&self->lock);
        dev_info(&client->dev, "%s: done", __func__);
        return 0;
}
