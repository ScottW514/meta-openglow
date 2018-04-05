/**
 * pic_private.h
 *
 * Private header for the PIC driver.
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

#ifndef KERNEL_SRC_PIC_PRIVATE_H_
#define KERNEL_SRC_PIC_PRIVATE_H_

#include "pic.h"
#include "notifiers.h"
#include "uapi/openglow.h"
#include <linux/leds.h>

struct pic_transaction {
  pic_command cmd;
  pic_value value;
} __attribute__((packed));


/**
 * Private data members.
 */
struct pic {
  /** Device that owns this data */
  struct device *dev;
  /** Lock to ensure mutually exclusive access */
  struct mutex lock;
  /** Notifiers */
  struct notifier_block dms_notifier;
  /** LED devices */
  struct led_classdev lid_led;
  struct led_classdev button_led_1;
  struct led_classdev button_led_2;
  struct led_classdev button_led_3;
};


/* defined in pic_api.c */
extern const struct attribute_group pic_attr_group;

/* defined in pic.c */
int pic_read_one_register(struct i2c_client *client, enum pic_register reg);
int pic_write_one_register(struct i2c_client *client, enum pic_register reg, pic_value new_value);

/* defined in pic_leds.c */
int pic_register_leds(struct i2c_client *client);
void pic_unregister_leds(struct i2c_client *client);

#endif
