/**
 * pic.c
 *
 * I2C driver for the PIC that acts as an analog I/O controller.
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

#include "pic_private.h"
#include "uapi/openglow.h"
#include "notifiers.h"

extern struct kobject *openglow_kobj;

/** Module parameters */
extern int pic_enabled;


int pic_read_one_register(struct i2c_client *client, enum pic_register reg)
{
  struct pic *self = i2c_get_clientdata(client);
  int ret;
  mutex_lock(&self->lock);
  ret = i2c_smbus_read_word_data(client, reg);
  mutex_unlock(&self->lock);
  return ret;
}


int pic_write_one_register(struct i2c_client *client, enum pic_register reg, pic_value new_value)
{
  struct pic *self = i2c_get_clientdata(client);
  int ret;
  int read_back;
  mutex_lock(&self->lock);
  ret = i2c_smbus_write_word_data(client, reg, new_value);
  if (ret == 0) {
    read_back = i2c_smbus_read_word_data(client, reg);
    if (new_value == read_back) {
      ret = 0;
    } else {
      dev_err(&client->dev, "attempted to write value %d to register %d but read back %d", new_value, reg, read_back);
      ret = -EIO;
    }
  }
  mutex_unlock(&self->lock);
  return ret;
}

/* Deadman switch trip notification */
static void pic_make_safe(struct i2c_client *client)
{
  // /* Zero out everything */
  pic_write_one_register(client, PIC_REG_STEP_DAC_X, 0);
  pic_write_one_register(client, PIC_REG_STEP_DAC_Y, 0);
  pic_write_one_register(client, PIC_REG_LID_LED, 0);
  pic_write_one_register(client, PIC_REG_BUTTON_LED1, 0);
  pic_write_one_register(client, PIC_REG_BUTTON_LED2, 0);
  pic_write_one_register(client, PIC_REG_BUTTON_LED3, 0);
  dev_err(&client->dev, "making safe");
}


static int pic_dms_handler(struct notifier_block *nb, unsigned long action, void *data)
{
  struct pic *self = container_of(nb, struct pic, dms_notifier);
  struct i2c_client *client = to_i2c_client(self->dev);
  pic_make_safe(client);
  return 0;
}


int pic_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  struct pic *self;
  int ret;
  dev_info(&client->dev, "%s: started", __func__);
  if (!pic_enabled) { dev_info(&client->dev, "%s: disabled, skipping", __func__); return 0; }
  self = devm_kzalloc(&client->dev, sizeof(*self), GFP_KERNEL);
  if (!self) {
    return -ENOMEM;
  }
  self->dev = &client->dev;
  mutex_init(&self->lock);
  i2c_set_clientdata(client, self);

  /* Detect the device */
  ret = pic_read_one_register(client, PIC_REG_ID);
  if (ret < 0) {
    dev_err(&client->dev, "failed to communicate with device, error %d", ret);
    goto failed_pic_detect;
  }
  if (ret != PIC_MAGIC_NUMBER) {
    dev_err(&client->dev, "failed to detect device: returned ID %04hx, expected %04hx", ret, PIC_MAGIC_NUMBER);
    ret = -ENODEV;
    goto failed_pic_detect;
  } else {
    dev_dbg(&client->dev, "found device, ID=%04hx", ret);
  }

  /* Register the LED devices */
  ret = pic_register_leds(client);
  if (ret < 0) {
    dev_err(&client->dev, "failed to register LEDs");
    goto failed_register_leds;
  }

  /* Create sysfs attributes */
  ret = sysfs_create_group(&client->dev.kobj, &pic_attr_group);
  if (ret < 0) {
    dev_err(&client->dev, "failed to register attribute group");
    goto failed_create_group;
  }

  /* Add a link in /sys/openglow */
  ret = sysfs_create_link(openglow_kobj, &client->dev.kobj, PIC_GROUP_NAME);
  if (ret) {
    goto failed_create_link;
  }

  /* Add deadman switch notifier */
  self->dms_notifier.notifier_call = pic_dms_handler;
  dms_notifier_chain_register(&dms_notifier_list, &self->dms_notifier);

  /* Set initial values */
  pic_write_one_register(client, PIC_REG_STEP_DAC_X, ATTR_X_CURRENT_INIT);
  pic_write_one_register(client, PIC_REG_STEP_DAC_Y, ATTR_Y_CURRENT_INIT);

  dev_info(&client->dev, "%s: done", __func__);
  return 0;

failed_create_link:
  sysfs_remove_group(&client->dev.kobj, &pic_attr_group);
failed_create_group:
  pic_unregister_leds(client);
failed_register_leds:
failed_pic_detect:
  mutex_destroy(&self->lock);
  return ret;
}


int pic_remove(struct i2c_client *client)
{
  struct pic *self = i2c_get_clientdata(client);
  if (!pic_enabled) { return 0; }
  dev_info(&client->dev, "%s: started", __func__);
  pic_make_safe(client);
  dms_notifier_chain_unregister(&dms_notifier_list, &self->dms_notifier);
  sysfs_remove_link(&client->dev.kobj, PIC_GROUP_NAME);
  sysfs_remove_group(&client->dev.kobj, &pic_attr_group);
  pic_unregister_leds(client);
  mutex_destroy(&self->lock);
  dev_info(&client->dev, "%s: done", __func__);
  return 0;
}
