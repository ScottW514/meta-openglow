/**
 * OpenGlow kernel module
 *
 * Copyright (C) 2018 Scott Wiederhold <s.e.wiederhold@gmail.com>
 * Portions Copyright (C) 2015-2018 Glowforge, Inc. <opensource@glowforge.com>
 * Portions Written by Matt Sarnoff with contributions from Taylor Vaughn.
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
#include "cnc.h"
#include "pic.h"
#include "thermal.h"
#include "openglow.h"

/** Module parameters */
int cnc_enabled = 1;
int pic_enabled = 1;
int thermal_enabled = 1;

module_param(cnc_enabled, int, 0);
module_param(pic_enabled, int, 0);
module_param(thermal_enabled, int, 0);

/** kobject that provides /sys/openglow */
struct kobject *openglow_kobj;

ATOMIC_NOTIFIER_HEAD(dms_notifier_list);

static const struct of_device_id pic_dt_ids[] = {
        { .compatible = "openglow,pic" },
        {},
};

static struct i2c_device_id pic_idtable[] = {
        { "openglow_pic", 0 },
        {},
};

static struct i2c_driver pic_driver = {
        .probe =  pic_probe,
        .remove = pic_remove,
        .id_table = pic_idtable,
        .driver = {
                .name =   "openglow_pic",
                .owner =  THIS_MODULE,
                .of_match_table = of_match_ptr(pic_dt_ids),
        },
};

static struct of_device_id cnc_dt_ids[] = {
        { .compatible = "openglow,cnc" },
        {}
};


static struct platform_driver cnc = {
        .probe  = cnc_probe,
        .remove = cnc_remove,
        .driver = {
                .name = "openglow_cnc",
                .owner = THIS_MODULE,
                .of_match_table = of_match_ptr(cnc_dt_ids)
        }
};

static struct of_device_id thermal_dt_ids[] = {
        { .compatible = "openglow,thermal" },
        {},
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

        /* Initialize the PIC */
        status = i2c_add_driver(&pic_driver);
        if (status < 0) {
                pr_err("failed to initialize PIC driver\n");
                goto failed_pic_init;
        }

        /* Initialize the thermal subsystem */
        status = platform_driver_register(&thermal);
        if (status < 0) {
                pr_err("failed to initialize thermal controller\n");
                goto failed_thermal_init;
        }

        /* Initialize the stepper driver */
        status = platform_driver_register(&cnc);
        if (status < 0) {
                pr_err("failed to initialize CNC driver\n");
                goto failed_cnc_init;
        }

        pr_info("%s: done\n", __func__);
        return 0;

failed_cnc_init:
        platform_driver_unregister(&thermal);
failed_thermal_init:
        i2c_del_driver(&pic_driver);
failed_pic_init:
        kobject_put(openglow_kobj);
        return status;
}
module_init(openglow_init);


static void __exit openglow_exit(void)
{
        pr_info("%s: started\n", __func__);
        platform_driver_unregister(&cnc);
        platform_driver_unregister(&thermal);
        i2c_del_driver(&pic_driver);
        kobject_put(openglow_kobj);
        pr_info("%s: done\n", __func__);
}
module_exit(openglow_exit);


/** For autoloading */
static struct of_device_id openglow_dt_ids[] = {
        { .compatible = "openglow,cnc" },
        { .compatible = "openglow,pic" },
        { .compatible = "openglow,thermal" },
        {}
};
MODULE_DEVICE_TABLE(of, openglow_dt_ids);
MODULE_DEVICE_TABLE(i2c, pic_idtable);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Scott Wiederhold <s.e.wiederhold@gmail.com>");
MODULE_DESCRIPTION("OpenGlow Driver");
MODULE_VERSION("dev");
