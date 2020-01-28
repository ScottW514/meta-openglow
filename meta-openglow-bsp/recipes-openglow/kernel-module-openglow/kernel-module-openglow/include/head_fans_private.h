/**
 * head_fans_private.h
 *
 * Private header for the head fan interface.
 *
 * Copyright (C) 2018 Scott Wiederhold s.e.wiederhold@gmail.com>
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

#ifndef KERNEL_SRC_HEAD_FANS_PRIVATE_H_
#define KERNEL_SRC_HEAD_FANS_PRIVATE_H_

#include "head_fans.h"
#include "notifiers.h"

enum {
        FAN_AIR_ASSIST,
        FAN_LENS_PURGE,
        HEAD_FANS_NUM_FANS
};

struct fan_config {
        const char *name;
};

struct head_fans {
        /** Device that owns this data */
        struct device *dev;
        /** Lock to ensure mutually exclusive access */
        struct mutex lock;
        /** Fan Channels **/
        int fans[HEAD_FANS_NUM_FANS];
        /** Notifiers */
        struct notifier_block dms_notifier;
};

#endif
