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

static ssize_t tmc2130_spi_transfer(struct spi_device *spi, uint8_t cmd, uint32_t *value)
{
        struct tmc2130 *self = spi_get_drvdata(spi);
        char str[11];
        WORD32.allfields = ((value) ? *value : 0);
        char tx[] = {cmd, WORD32.b0, WORD32.b1, WORD32.b2, WORD32.b3};
        char rx[] = {0, 0, 0, 0, 0};
        struct spi_transfer tfer[] = {{.tx_buf = tx, .rx_buf = rx, .len = ARRAY_SIZE(tx)}};

        if (spi_sync_transfer(spi, tfer, ARRAY_SIZE(tfer))) { return -EINVAL; }
        self->status = rx[0];

        if (value) {
                sprintf(str, "0x%02x%02x%02x%02x", rx[1], rx[2], rx[3], rx[4]);
                sscanf(str, "%i", value);
        }
        return 0;
}

static ssize_t tmc2130_status_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        struct spi_device *spi = to_spi_device(dev);
        struct tmc2130 *self = spi_get_drvdata(spi);
        ssize_t outlen = 0;
        uint32_t value, tstep, mscnt, pwm_scale, lost_steps;

        mutex_lock(&self->lock);
        /* Send DRV_STATUS Command */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_DRV_STATUS, &value)) { return -EINVAL; }
        SPI_STATUS.allfields = self->status;
        /* Send GSTAT Command, Retrieve DRV_STATUS */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_GSTAT, &value)) { return -EINVAL; }
        DRV_STATUS.allfields = value;
        // /* Send IOIN Command, Retrieve GSTAT */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_IOIN, &value)) { return -EINVAL; }
        GSTAT.allfields = value;
        // /* Send TSTEP Command, Retrieve IOIN */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_TSTEP, &value)) { return -EINVAL; }
        IOIN.allfields = value;
        // /* Send MSCNT Command, Retrieve TSTEP */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_MSCNT, &tstep)) { return -EINVAL; }
        // /* Send MSCURACT Command, Retrieve MSCNT */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_MSCURACT, &mscnt)) { return -EINVAL; }
        // /* Send PWM_SCALE Command, Retrieve MSCURACT */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_PWM_SCALE, &value)) { return -EINVAL; }
        MSCURACT.allfields = value;
        // /* Send LOST_STEPS Command, Retrieve PWM_SCALE */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_LOST_STEPS, &pwm_scale)) { return -EINVAL; }
        // /* Send LOST_STEPS Command, Retrieve LOST_STEPS */
        if (tmc2130_spi_transfer(spi, TMC2130_REG_LOST_STEPS, &lost_steps)) { return -EINVAL; }
        mutex_unlock(&self->lock);

        outlen = scnprintf(buf+outlen, PAGE_SIZE-outlen,
		"HW VERSION:%x\n",
                IOIN.VERSION
	);
        outlen += scnprintf(buf+outlen, PAGE_SIZE-outlen,
		"SPI_STATUS: standstill:%u  sg2:%u  driver_error:%u  reset_flag:%u\n",
                SPI_STATUS.standstill,
                SPI_STATUS.sg2,
                SPI_STATUS.driver_error,
                SPI_STATUS.reset_flag
	);
        outlen += scnprintf(buf+outlen, PAGE_SIZE-outlen,
		"GSTAT: reset:%u  drv_err:%u  uv_cp:%u\n",
                GSTAT.reset,
                GSTAT.drv_err,
                GSTAT.uv_cp
	);
        outlen += scnprintf(buf+outlen, PAGE_SIZE-outlen,
                "DRV_STATUS: stst:%u ola:%u olb:%u s2ga:%u s2gb:%u\n",
                DRV_STATUS.stst,
                DRV_STATUS.ola,
                DRV_STATUS.olb,
                DRV_STATUS.s2ga,
                DRV_STATUS.s2gb
        );
        outlen += scnprintf(buf+outlen, PAGE_SIZE-outlen,
                "            otpw:%u ot:%u CSACTUAL:%u fsactive:%u\n",
                DRV_STATUS.otpw,
                DRV_STATUS.ot,
                DRV_STATUS.CSACTUAL,
                DRV_STATUS.fsactive
        );
        outlen += scnprintf(buf+outlen, PAGE_SIZE-outlen,
                "            stallGuard:%u SG_RESULT:%u\n",
                DRV_STATUS.stallGuard,
                DRV_STATUS.SG_RESULT
        );
        outlen += scnprintf(buf+outlen, PAGE_SIZE-outlen,
		"IOIN: DCO:%u  DRV_ENN:%u  DCIN:%u DCEN:%u DIR:%u STEP:%u\n",
                IOIN.DCO,
                IOIN.DRV_ENN_CFG6,
                IOIN.DCIN_CFG5,
                IOIN.DCEN_CFG4,
                IOIN.DIR,
                IOIN.STEP
        );
        outlen += scnprintf(buf+outlen, PAGE_SIZE-outlen,
		"TSTEP:%07u MSCNT:%04u  PWM_SCALE:%03u  LOST_STEPS:%04u\n",
                tstep, mscnt, pwm_scale, lost_steps
        );
        outlen += scnprintf(buf+outlen, PAGE_SIZE-outlen,
		"MSCURACT A:%03d  B:%03d\n",
                MSCURACT.cur_a,
                MSCURACT.cur_b
        );
        return outlen;
}
static DEVICE_ATTR(status, S_IRUSR, tmc2130_status_show, NULL);

static ssize_t tmc2130_read_register_ascii(struct device *dev, enum tmc2130_register reg, char *buf)
{
        struct spi_device *spi = to_spi_device(dev);
        struct tmc2130 *self = spi_get_drvdata(spi);
        uint32_t value;

        mutex_lock(&self->lock);
        if (tmc2130_spi_transfer(spi, reg, NULL)) { return -EINVAL; }
        if (tmc2130_spi_transfer(spi, reg, &value)) { return -EINVAL; }
        mutex_unlock(&self->lock);
        return scnprintf(buf, PAGE_SIZE, "0x%08x\n", value);
}

static ssize_t tmc2130_write_register_ascii(struct device *dev, enum tmc2130_register reg, const char *buf, size_t count)
{
        struct spi_device *spi = to_spi_device(dev);
        struct tmc2130 *self = spi_get_drvdata(spi);
        uint32_t value = 0;

        if (count >= PAGE_SIZE) { return -E2BIG; }
        if (sscanf(buf, "%i", &value) != 1) { return -EINVAL; }
        if (value < 0 || value > 0xFFFFFFFF) { return -EINVAL; }

        mutex_lock(&self->lock);
        if (tmc2130_spi_transfer(spi, reg | 0x80, &value)) { return -EINVAL; }
        mutex_unlock(&self->lock);
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
        DEV_ATTR_PTR(ATTR_STATUS),
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
        if (ret)
                goto failed_create_link;

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
