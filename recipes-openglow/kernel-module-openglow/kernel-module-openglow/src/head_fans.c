/**
 * head_fans.c
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
#include "head_fans_private.h"
#include "device_attr.h"
#include "openglow.h"

#define SEL_FAN(fan, reg) (reg + fan * 0x10)

extern struct kobject *openglow_kobj;

/** Module parameters */
extern int head_fans_enabled;


static const struct fan_config fan_configs[HEAD_FANS_NUM_FANS] = {
	[FAN_AIR_ASSIST]     = {"air-assist-fan"},
	[FAN_LENS_PURGE]     = {"lens-purge-fan"},
};


static uint8_t head_fans_read_i2c_byte(struct i2c_client *client, int reg)
{
        struct head_fans *self = i2c_get_clientdata(client);
        uint8_t ret;
        mutex_lock(&self->lock);
        ret = i2c_smbus_read_byte_data(client, reg);
        mutex_unlock(&self->lock);
        return ret;
}


static uint16_t head_fans_read_i2c_word(struct i2c_client *client, int reg)
{
        struct head_fans *self = i2c_get_clientdata(client);
        int ret;
        mutex_lock(&self->lock);
        ret = i2c_smbus_read_word_data(client, reg);
	ret = (ret >> 8) | (ret << 8);
        mutex_unlock(&self->lock);
        return ret;
}


static int head_fans_write_i2c_byte(struct i2c_client *client, int reg, uint8_t new_value)
{
        struct head_fans *self = i2c_get_clientdata(client);
        int ret;
        mutex_lock(&self->lock);
        ret = i2c_smbus_write_byte_data(client, reg, new_value);
        mutex_unlock(&self->lock);
        return ret;
}


static void head_fans_make_safe(struct head_fans *self)
{
        int i;
        struct i2c_client *client = to_i2c_client(self->dev);
	i2c_smbus_write_byte_data(client, SEL_FAN(FAN_AIR_ASSIST, REG_FAN_SETTING), 0x00);
	i2c_smbus_write_byte_data(client, SEL_FAN(FAN_LENS_PURGE, REG_FAN_SETTING), 0x00);
	dev_err(self->dev, "making safe");
}


static int head_fans_dms_handler(struct notifier_block *nb, unsigned long action, void *data)
{
        struct head_fans *self = container_of(nb, struct head_fans, dms_notifier);
        head_fans_make_safe(self);
        return 0;
}


static ssize_t head_fans_read_register_ascii(struct device *dev, int reg, char *buf)
{
        struct i2c_client *client = to_i2c_client(dev);
        ssize_t value = head_fans_read_i2c_byte(client, reg);
        if (value >= 0) {
                return scnprintf(buf, PAGE_SIZE, "%hu\n", value);
        } else {
                return value;
        }
}


static ssize_t head_fans_read_tach_ascii(struct device *dev, int reg, char *buf)
{
        struct i2c_client *client = to_i2c_client(dev);
        ssize_t value = head_fans_read_i2c_word(client, reg) >> 3;
	int rpm = (3932160 * 2) / value;
	//  rpm = rpm > 999 ? rpm : 0;
        if (value >= 0) {
                return scnprintf(buf, PAGE_SIZE, "%hu\n", rpm);
        } else {
                return value;
        }
}


static ssize_t head_fans_write_register_ascii(struct device *dev, int reg, const char *buf, size_t count)
{
        struct i2c_client *client = to_i2c_client(dev);
        uint8_t new_value = 0;
        int ret;
        /* Error if the entire buffer is used; we need a null-terminator */
        if (count >= PAGE_SIZE) { return -E2BIG; }

        if (sscanf(buf, "%hhu", &new_value) != 1) {
                return -EINVAL;
        }
        ret = head_fans_write_i2c_byte(client, reg, new_value);
	return (ret >= 0) ? count : ret;
}


#define DEFINE_FAN_TACH_ATTR(name, reg) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                return head_fans_read_tach_ascii(dev, reg, buf); } \
        static DEVICE_ATTR(name, S_IRUSR, name##_show, NULL)

#define DEFINE_FAN_PWM_ATTR(name, reg) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                return head_fans_read_register_ascii(dev, reg, buf); } \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                return head_fans_write_register_ascii(dev, reg, buf, count); } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

DEFINE_FAN_PWM_ATTR(ATTR_AIR_ASSIST_PWM,   SEL_FAN(FAN_AIR_ASSIST, REG_FAN_SETTING));
DEFINE_FAN_PWM_ATTR(ATTR_LENS_PURGE_PWM,   SEL_FAN(FAN_LENS_PURGE, REG_FAN_SETTING));
DEFINE_FAN_TACH_ATTR(ATTR_AIR_ASSIST_TACH, SEL_FAN(FAN_AIR_ASSIST, REG_TACH_READ_HIGH));

static struct attribute *head_fans_attrs[] = {
	DEV_ATTR_PTR(ATTR_AIR_ASSIST_PWM),
	DEV_ATTR_PTR(ATTR_LENS_PURGE_PWM),
	DEV_ATTR_PTR(ATTR_AIR_ASSIST_TACH),
        NULL
};


const struct attribute_group head_fans_attr_group = {
        .attrs = head_fans_attrs
};


int head_fans_init_fans(struct device_node *of_node, const struct fan_config *fan_configs, int *fans, size_t nfans)
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


int head_fans_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        struct head_fans *self;
        int ret = 0;
        if (!head_fans_enabled) { dev_info(&client->dev, "%s: disabled, skipping", __func__); return 0; }
        dev_info(&client->dev, "%s: started", __func__);

        /* Allocate driver data */
        self = devm_kzalloc(&client->dev, sizeof(*self), GFP_KERNEL);
        if (!self) {
                return -ENOMEM;
        }
        self->dev = &client->dev;
	mutex_init(&self->lock);
        i2c_set_clientdata(client, self);

	/* Set up FANs */
        ret = head_fans_init_fans(client->dev.of_node, fan_configs, self->fans, HEAD_FANS_NUM_FANS);
        if (ret) {
                goto failed_fan_init;
        }
	int manufacturer = i2c_smbus_read_byte_data(client, REG_MANUFACTURER_ID);
	int product_id = i2c_smbus_read_byte_data(client, REG_PRODUCT_ID);
	if (product_id != 0x36 || manufacturer != 0x5D) {
		dev_err(&client->dev, "fan controller not detected\n");
                ret = -1;
                goto failed_fan_init;
	}
	/* PWM Outputs to Push-Pull */
	ret = i2c_smbus_write_byte_data(client, REG_PWM_OUTPUT_CONFIG, 0x07);
	/* Set all fan outputs to 0 */
	ret += i2c_smbus_write_byte_data(client, SEL_FAN(FAN_AIR_ASSIST, REG_FAN_SETTING), 0x00);
	ret += i2c_smbus_write_byte_data(client, SEL_FAN(FAN_LENS_PURGE, REG_FAN_SETTING), 0x00);
	if (ret < 0) {
                dev_err(&client->dev, "failed to configure fan controller");
                goto failed_fan_init;
        }

        /* Create sysfs attributes */
        ret = sysfs_create_group(&client->dev.kobj, &head_fans_attr_group);
        if (ret < 0) {
                dev_err(&client->dev, "failed to register attribute group");
                goto failed_create_group;
        }

        /* Add a link in /sys/openglow */
        ret = sysfs_create_link(openglow_kobj, &client->dev.kobj, HEAD_FANS_GROUP_NAME);
        if (ret) {
                goto failed_create_link;
        }

        /* Add deadman switch notifier */
        self->dms_notifier.notifier_call = head_fans_dms_handler;
        dms_notifier_chain_register(&dms_notifier_list, &self->dms_notifier);

        dev_info(&client->dev, "%s: done", __func__);
        return 0;

failed_create_link:
        sysfs_remove_group(&client->dev.kobj, &head_fans_attr_group);
failed_create_group:
failed_fan_init:
        return ret;
}


int head_fans_remove(struct i2c_client *client)
{
        struct head_fans *self = i2c_get_clientdata(client);
        if (!head_fans_enabled) { return 0; }
        dev_info(&client->dev, "%s: started", __func__);
        head_fans_make_safe(self);
        dms_notifier_chain_unregister(&dms_notifier_list, &self->dms_notifier);
        sysfs_remove_link(&client->dev.kobj, HEAD_FANS_GROUP_NAME);
        sysfs_remove_group(&client->dev.kobj, &head_fans_attr_group);
	mutex_destroy(&self->lock);
        dev_info(&client->dev, "%s: done", __func__);
        return 0;
}
