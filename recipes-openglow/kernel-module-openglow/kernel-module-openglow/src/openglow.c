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
#include "head_fans.h"
#include "leds.h"
#include "thermal.h"
#include "tmc2130.h"
#include "openglow.h"

/** Module parameters */
int cnc_enabled = 1;
int head_fans_enabled = 1;
int leds_enabled = 1;
int thermal_enabled = 1;
int tmc2130_enabled = 1;

module_param(cnc_enabled, int, 0);
module_param(head_fans_enabled, int, 0);
module_param(leds_enabled, int, 0);
module_param(thermal_enabled, int, 0);
module_param(tmc2130_enabled, int, 0);

/** kobject that provides /sys/openglow */
struct kobject *openglow_kobj;

ATOMIC_NOTIFIER_HEAD(dms_notifier_list);

static struct of_device_id cnc_dt_ids[] = {
        { .compatible = "openglow,cnc" },
        {},
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

static const struct of_device_id head_fans_dt_ids[] = {
        { .compatible = "openglow,head_fans" },
        {},
};

static struct i2c_device_id head_fans_idtable[] = {
        { "openglow_head_fans", 0 },
        {},
};

static struct i2c_driver head_fans = {
        .probe =  head_fans_probe,
        .remove = head_fans_remove,
        .id_table = head_fans_idtable,
        .driver = {
                .name =   "openglow_head_fans",
                .owner =  THIS_MODULE,
                .of_match_table = of_match_ptr(head_fans_dt_ids),
        },
};

static const struct of_device_id thermal_dt_ids[] = {
        { .compatible = "openglow,thermal" },
        {},
};

static struct i2c_device_id thermal_idtable[] = {
        { "openglow_thermal", 0 },
        {},
};

static struct i2c_driver thermal = {
        .probe =  thermal_probe,
        .remove = thermal_remove,
        .id_table = thermal_idtable,
        .driver = {
                .name =   "openglow_thermal",
                .owner =  THIS_MODULE,
                .of_match_table = of_match_ptr(thermal_dt_ids),
        },
};

static const struct of_device_id leds_dt_ids[] = {
        { .compatible = "openglow,leds" },
        {},
};

static struct i2c_device_id leds_idtable[] = {
        { "openglow_leds", 0 },
        {},
};

static struct i2c_driver leds = {
        .probe =  leds_probe,
        .remove = leds_remove,
        .id_table = leds_idtable,
        .driver = {
                .name =   "openglow_leds",
                .owner =  THIS_MODULE,
                .of_match_table = of_match_ptr(leds_dt_ids),
        },
};

static struct of_device_id tmc2130_dt_ids[] = {
        { .compatible = "openglow,tmc2130" },
        {},
};

static struct spi_driver tmc2130 = {
        .probe  = tmc2130_probe,
        .remove = tmc2130_remove,
        .driver = {
                .name = "openglow_tmc2130",
                .owner = THIS_MODULE,
                .of_match_table = of_match_ptr(tmc2130_dt_ids)
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
        status = i2c_add_driver(&thermal);
        if (status < 0) {
                pr_err("failed to initialize thermal controller\n");
                goto failed_thermal_init;
        }

        /* Initialize the head fan subsystem */
        status = i2c_add_driver(&head_fans);
        if (status < 0) {
                pr_err("failed to initialize head fan controller\n");
                goto failed_head_fans_init;
        }

        /* Initialize the LED subsystem */
        status = i2c_add_driver(&leds);
        if (status < 0) {
                pr_err("failed to initialize led controller\n");
                goto failed_leds_init;
        }

        /* Initialize the cnc driver */
        status = platform_driver_register(&cnc);
        if (status < 0) {
                pr_err("failed to initialize CNC driver\n");
                goto failed_cnc_init;
        }

        /* Initialize the stepper driver */
        status = spi_register_driver(&tmc2130);
        if (status < 0) {
          pr_err("failed to initialize TMC2130 driver\n");
          goto failed_tmc2130_init;
        }

        pr_info("%s: done\n", __func__);
        return 0;

failed_tmc2130_init:
        platform_driver_unregister(&cnc);
failed_cnc_init:
        i2c_del_driver(&leds);
failed_leds_init:
        i2c_del_driver(&thermal);
failed_head_fans_init:
failed_thermal_init:
        kobject_put(openglow_kobj);
        return status;
}
module_init(openglow_init);


static void __exit openglow_exit(void)
{
        pr_info("%s: started\n", __func__);
        spi_unregister_driver(&tmc2130);
        platform_driver_unregister(&cnc);
        i2c_del_driver(&leds);
        i2c_del_driver(&thermal);
        i2c_del_driver(&head_fans);
        kobject_put(openglow_kobj);
        pr_info("%s: done\n", __func__);
}
module_exit(openglow_exit);


/** For autoloading */
static struct of_device_id openglow_dt_ids[] = {
        { .compatible = "openglow,tmc2130" },
        { .compatible = "openglow,cnc" },
        { .compatible = "openglow,head_fans" },
        { .compatible = "openglow,leds" },
        { .compatible = "openglow,thermal" },
        {}
};
MODULE_DEVICE_TABLE(of, openglow_dt_ids);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Scott Wiederhold <s.e.wiederhold@gmail.com>");
MODULE_DESCRIPTION("OpenGlow Driver");
MODULE_VERSION("dev");
