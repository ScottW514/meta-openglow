/**
 * tmc2130.c
 *
 * Trinamic TMC2130 Driver
 *
 * Copyright (C) 2018 Scott Wiederhold <s.e.wiederhold@gmail.com>
 * Based on work by Glowforge, Inc. <opensource@glowforge.com>
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

#include "tmc2130_private.h"
#include "device_attr.h"

#include <linux/of.h>
#include <linux/types.h>

extern struct kobject *openglow_kobj;

/** Module parameters */
extern int tmc2130_enabled;


static ssize_t tmc2130_read_register_ascii(struct device *dev, enum tmc2130_register reg, char *buf)
{
        struct spi_device *spi = to_spi_device(dev);
        struct tmc2130 *self = spi_get_drvdata(spi);
        const char tx[] = {reg, 0x00, 0x00, 0x00, 0x00};
        char rx[5];
        struct spi_transfer tfer[] = {
                {.tx_buf = tx, .len = sizeof(tx),.rx_buf = rx, .len = sizeof(tx)},
        };

        mutex_lock(&self->lock);
        spi_sync_transfer(spi, tfer, ARRAY_SIZE(tfer));
        spi_sync_transfer(spi, tfer, ARRAY_SIZE(tfer));
        mutex_unlock(&self->lock);
        self->status = rx[0];
        return scnprintf(buf, PAGE_SIZE,
                "status: 0x%02x\nvalue : 0x%02x%02x%02x%02x\n",
                rx[0], rx[1], rx[2], rx[3], rx[4]);
}


static ssize_t tmc2130_write_register_ascii(struct device *dev, enum tmc2130_register reg, const char *buf, size_t count)
{
        struct spi_device *spi = to_spi_device(dev);
        struct tmc2130 *self = spi_get_drvdata(spi);
        unsigned long new_value = 0;

        if (count >= PAGE_SIZE) { return -E2BIG; }

        if (sscanf(buf, "%hi", &new_value) != 1) { return -EINVAL; }

        if (new_value < 0 || new_value > 0xFFFFFFFF) { return -EINVAL; }

        char tx[] = {
                reg | 0x80,
                (int)((new_value & 0xFF000000) >> 24),
                (int)((new_value & 0x00FF0000) >> 16),
                (int)((new_value & 0x0000FF00) >> 8),
                (int)(new_value  & 0x000000FF)
        };

        char rx[5];
        struct spi_transfer tfer[] = {
                {.tx_buf = tx, .len = sizeof(tx),.rx_buf = rx, .len = sizeof(tx)}
        };

        mutex_lock(&self->lock);
        spi_sync_transfer(spi, tfer, ARRAY_SIZE(tfer));
        mutex_unlock(&self->lock);

        self->status = rx[0];
        return count;
}


#define DEFINE_READONLY_REG_ATTR(name, reg) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                return tmc2130_read_register_ascii(dev, reg, buf); } \
        static DEVICE_ATTR(name, S_IRUSR, name##_show, NULL)

#define DEFINE_READWRITE_REG_ATTR(name, reg) \
        static ssize_t name##_show(struct device *dev, struct device_attribute *attr, char *buf) { \
                return tmc2130_read_register_ascii(dev, reg, buf); } \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                return tmc2130_write_register_ascii(dev, reg, buf, count); } \
        static DEVICE_ATTR(name, S_IRUSR|S_IWUSR, name##_show, name##_store)

#define DEFINE_WRITEONLY_REG_ATTR(name, reg) \
        static ssize_t name##_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) { \
                return tmc2130_write_register_ascii(dev, reg, buf, count); } \
        static DEVICE_ATTR(name, S_IWUSR, NULL, name##_store)

/** Single register attributes (ASCII decimal values) */
DEFINE_READWRITE_REG_ATTR(ATTR_GCONF, TMC2130_REG_GCONF);
DEFINE_READWRITE_REG_ATTR(ATTR_XDIRECT, TMC2130_REG_XDIRECT);
DEFINE_READWRITE_REG_ATTR(ATTR_CHOPCONF, TMC2130_REG_CHOPCONF);

DEFINE_READONLY_REG_ATTR(ATTR_GSTAT, TMC2130_REG_GSTAT);
DEFINE_READONLY_REG_ATTR(ATTR_IOIN, TMC2130_REG_IOIN);
DEFINE_READONLY_REG_ATTR(ATTR_TSTEP, TMC2130_REG_TSTEP);
DEFINE_READONLY_REG_ATTR(ATTR_MSCNT, TMC2130_REG_MSCNT);
DEFINE_READONLY_REG_ATTR(ATTR_MSCURACT, TMC2130_REG_MSCURACT);
DEFINE_READONLY_REG_ATTR(ATTR_DRV_STATUS, TMC2130_REG_DRV_STATUS);
DEFINE_READONLY_REG_ATTR(ATTR_PWM_SCALE, TMC2130_REG_PWM_SCALE);
DEFINE_READONLY_REG_ATTR(ATTR_LOST_STEPS, TMC2130_REG_LOST_STEPS);

DEFINE_WRITEONLY_REG_ATTR(ATTR_IHOLD_IRUN, TMC2130_REG_IHOLD_IRUN);
DEFINE_WRITEONLY_REG_ATTR(ATTR_TPOWERDOWN, TMC2130_REG_TPOWERDOWN);
DEFINE_WRITEONLY_REG_ATTR(ATTR_TPWMTHRS, TMC2130_REG_TPWMTHRS);
DEFINE_WRITEONLY_REG_ATTR(ATTR_TCOOLTHRS, TMC2130_REG_TCOOLTHRS);
DEFINE_WRITEONLY_REG_ATTR(ATTR_THIGH, TMC2130_REG_THIGH);
DEFINE_WRITEONLY_REG_ATTR(ATTR_VDCMIN, TMC2130_REG_VDCMIN);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUT0, TMC2130_REG_MSLUT0);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUT1, TMC2130_REG_MSLUT1);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUT2, TMC2130_REG_MSLUT2);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUT3, TMC2130_REG_MSLUT3);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUT4, TMC2130_REG_MSLUT4);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUT5, TMC2130_REG_MSLUT5);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUT6, TMC2130_REG_MSLUT6);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUT7, TMC2130_REG_MSLUT7);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUTSEL, TMC2130_REG_MSLUTSEL);
DEFINE_WRITEONLY_REG_ATTR(ATTR_MSLUTSTART, TMC2130_REG_MSLUTSTART);
DEFINE_WRITEONLY_REG_ATTR(ATTR_COOLCONF, TMC2130_REG_COOLCONF);
DEFINE_WRITEONLY_REG_ATTR(ATTR_DCCTRL, TMC2130_REG_DCCTRL);
DEFINE_WRITEONLY_REG_ATTR(ATTR_PWMCONF, TMC2130_REG_PWMCONF);
DEFINE_WRITEONLY_REG_ATTR(ATTR_ENCM_CTRL, TMC2130_REG_ENCM_CTRL);

static struct attribute *tmc2130_attrs[] = {
        DEV_ATTR_PTR(ATTR_GCONF),
        DEV_ATTR_PTR(ATTR_GSTAT),
        DEV_ATTR_PTR(ATTR_IOIN),
        DEV_ATTR_PTR(ATTR_IHOLD_IRUN),
        DEV_ATTR_PTR(ATTR_TPOWERDOWN),
        DEV_ATTR_PTR(ATTR_TSTEP),
        DEV_ATTR_PTR(ATTR_TPWMTHRS),
        DEV_ATTR_PTR(ATTR_TCOOLTHRS),
        DEV_ATTR_PTR(ATTR_THIGH),
        DEV_ATTR_PTR(ATTR_XDIRECT),
        DEV_ATTR_PTR(ATTR_VDCMIN),
        DEV_ATTR_PTR(ATTR_MSLUT0),
        DEV_ATTR_PTR(ATTR_MSLUT1),
        DEV_ATTR_PTR(ATTR_MSLUT2),
        DEV_ATTR_PTR(ATTR_MSLUT3),
        DEV_ATTR_PTR(ATTR_MSLUT4),
        DEV_ATTR_PTR(ATTR_MSLUT5),
        DEV_ATTR_PTR(ATTR_MSLUT6),
        DEV_ATTR_PTR(ATTR_MSLUT7),
        DEV_ATTR_PTR(ATTR_MSLUTSEL),
        DEV_ATTR_PTR(ATTR_MSLUTSTART),
        DEV_ATTR_PTR(ATTR_MSCNT),
        DEV_ATTR_PTR(ATTR_MSCURACT),
        DEV_ATTR_PTR(ATTR_CHOPCONF),
        DEV_ATTR_PTR(ATTR_COOLCONF),
        DEV_ATTR_PTR(ATTR_DCCTRL),
        DEV_ATTR_PTR(ATTR_DRV_STATUS),
        DEV_ATTR_PTR(ATTR_PWMCONF),
        DEV_ATTR_PTR(ATTR_PWM_SCALE),
        DEV_ATTR_PTR(ATTR_ENCM_CTRL),
        DEV_ATTR_PTR(ATTR_LOST_STEPS),
        NULL
};

const struct attribute_group tmc2130_attr_group = {
        .attrs = tmc2130_attrs
};

int tmc2130_probe(struct spi_device *spi)
{
        struct tmc2130 *self;
        int ret;
        if (!tmc2130_enabled) { dev_info(&spi->dev, "%s: disabled, skipping", __func__); return 0; }
        dev_info(&spi->dev, "%s: started", __func__);
        dev_info(&spi->dev, "SPI bus %hd, cs %hhd, %d Hz, %d bits/word, mode 0x%hx",
                spi->master->bus_num,
                spi->chip_select,
                spi->max_speed_hz,
                spi->bits_per_word,
                spi->mode);

        self = devm_kzalloc(&spi->dev, sizeof(*self), GFP_KERNEL);
        if (!self) {
                return -ENOMEM;
        }
        self->dev = &spi->dev;
        mutex_init(&self->lock);
        spi_set_drvdata(spi, self);

        /* Create sysfs attributes */
        ret = sysfs_create_group(&spi->dev.kobj, &tmc2130_attr_group);
        if (ret < 0) {
          dev_err(&spi->dev, "failed to register attribute group");
          goto failed_create_group;
        }

        /* Add a link in /sys/openglow */
        struct device_node *np = self->dev->of_node;
        ret = sysfs_create_link(openglow_kobj, &spi->dev.kobj, np->name);
        if (ret) {
          goto failed_create_link;
        }

        dev_info(&spi->dev, "%s: done", __func__);
        return 0;

failed_create_link:
        sysfs_remove_group(&spi->dev.kobj, &tmc2130_attr_group);
failed_create_group:
        mutex_destroy(&self->lock);
        return ret;
}

int tmc2130_remove(struct spi_device *spi)
{
        struct tmc2130 *self = spi_get_drvdata(spi);
        struct device_node *np = self->dev->of_node;
        if (!tmc2130_enabled) { return 0; }
        dev_info(&spi->dev, "%s: started", __func__);
        sysfs_remove_link(&spi->dev.kobj, np->name);
        sysfs_remove_group(&spi->dev.kobj, &tmc2130_attr_group);
        mutex_destroy(&self->lock);
        dev_info(&spi->dev, "%s: done", __func__);
        return 0;
}
