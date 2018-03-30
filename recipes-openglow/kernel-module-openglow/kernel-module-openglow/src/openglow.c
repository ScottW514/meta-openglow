/**
 * OpenGlow kernel module
 *
 * Copyright (C) 2018 Scott Wiederhold <s.e.wiederhold@gmail.com>
 * Copyright (C) 2015-2018 Glowforge, Inc. <opensource@glowforge.com>
 * Written by Matt Sarnoff with contributions from Taylor Vaughn.
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
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/notifier.h>

#include "io.h"
#include "thermal.h"
#include "uapi/openglow.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Scott Wiederhold <s.e.wiederhold@gmail.com>");
MODULE_DESCRIPTION("OpenGlow Driver");
MODULE_VERSION("dev");

/** Module parameters */
int thermal_enabled = 1;

module_param(thermal_enabled, int, 0);

/** kobject that provides /sys/openglow */
struct kobject *openglow_kobj;

ATOMIC_NOTIFIER_HEAD(dms_notifier_list);



static struct of_device_id thermal_dt_ids[] = {
  { .compatible = "openglow,thermal" },
  {}
};


static struct platform_driver thermal = {
  .probe  = thermal_probe,
  .remove = thermal_remove,
  .driver = {
    .name = "openglow_thermal",
    .owner = THIS_MODULE,
    .of_match_table = of_match_ptr(thermal_dt_ids)
  }
};


static int __init openglow_init(void)
{
  int status = 0;

  pr_info("%s: started\n", __func__);

  /* Create /sys/openglow */
  openglow_kobj = kobject_create_and_add(ROOT_KOBJ_NAME, NULL);
  if (!openglow_kobj) {
    return -ENOMEM;
  }

  /* Initialize the thermal subsystem */
  status = platform_driver_register(&thermal);
  if (status < 0) {
    pr_err("failed to initialize thermal controller\n");
    goto failed_thermal_init;
  }

  pr_info("%s: done\n", __func__);
  return 0;

failed_thermal_init:
  kobject_put(openglow_kobj);
  return status;
}
module_init(openglow_init);


static void __exit openglow_exit(void)
{
  pr_info("%s: started\n", __func__);
  platform_driver_unregister(&thermal);
  kobject_put(openglow_kobj);
  pr_info("%s: done\n", __func__);
}
module_exit(openglow_exit);


/** For autoloading */
static struct of_device_id openglow_dt_ids[] = {
  { .compatible = "openglow,thermal" },
  {}
};
MODULE_DEVICE_TABLE(of, openglow_dt_ids);
