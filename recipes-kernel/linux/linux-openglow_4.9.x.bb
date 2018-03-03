# Adapted from linux-imx.inc, copyright (C) 2013, 2014 O.S. Systems Software LTDA
# Released under the MIT license (see COPYING.MIT for the terms)

require recipes-kernel/linux/linux-imx.inc

SUMMARY = "Linux kernel for OpenGlow boards"

SRC_URI = "git://github.com/boundarydevices/linux-imx6.git;branch=${SRCBRANCH} \
           file://git/arch/arm/boot/dts/openglow_std.dts \
           file://defconfig \
           file://config_1.cfg \
           file://config_2.cfg \
"

LOCALVERSION = "-1.0.0-ga+openglow"
SRCBRANCH = "boundary-imx_4.9.x_1.0.0_ga"
SRCREV = "${AUTOREV}"
DEPENDS += "lzop-native bc-native"
COMPATIBLE_MACHINE = "openglow_std"
