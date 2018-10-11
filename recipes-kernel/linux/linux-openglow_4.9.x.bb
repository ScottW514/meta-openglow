# Adapted from linux-imx.inc, copyright (C) 2013, 2014 O.S. Systems Software LTDA
# Released under the MIT license (see COPYING.MIT for the terms)

require recipes-kernel/linux/linux-imx.inc

SUMMARY = "Linux kernel for OpenGlow boards"

SRC_URI = "git://github.com/boundarydevices/linux-imx6.git;branch=${SRCBRANCH} \
           file://git/ \
           file://defconfig \
           file://0001-Add-DT-Prescaler-to-PWM-Driver.patch \
           file://0002-Add-EPIT-API.patch \
           file://0003-Add-OV5648-Driver.patch \
           file://0005-fix-rt-build.patch \
           file://0006-no-split-ptlocks.patch \
           file://0010-patch-4.9.128-rt94-rc1.patch \
"

LOCALVERSION = "-2.0.0-ga+openglow"
SRCBRANCH = "boundary-imx_4.9.x_2.0.0_ga"
SRCREV = "${AUTOREV}"
DEPENDS += "lzop-native bc-native"
COMPATIBLE_MACHINE = "openglow_std"
