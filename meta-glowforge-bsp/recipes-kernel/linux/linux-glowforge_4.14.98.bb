SUMMARY = "i.MX Linux Kernel"
DESCRIPTION = "i.MX Linux Kernel"

KBRANCH = "imx_4.14.98_2.3.0"
SRCREV = "0f549d8c4d5edd68a2a492afd48228458d3bca4a"
LINUX_VERSION = "4.14.98"
LINUX_VERSION_EXTENSION = "-glowforge"

inherit kernel
require recipes-kernel/linux/linux-yocto.inc

DEPENDS += "lzop-native bc-native"


KMETA = "kernel-meta"

SRC_URI += "git://source.codeaurora.org/external/imx/linux-imx;protocol=https;branch=${KBRANCH} \
            file://git/ \
            file://defconfig \
            file://0001-compiler-attributes-add-support-for-copy-gcc-9.patch \
            file://0002-include-linux-module.h-copy-init-exit-attrs-to-.patch \
            file://1001-Add-DT-Prescaler-to-PWM-Driver.patch \
            file://1002-Add-EPIT-API.patch \
            file://1003-Expose-SDMA-API.patch \
           "

DEFAULT_PREFERENCE = "1"
COMPATIBLE_MACHINE = "glowforge"
