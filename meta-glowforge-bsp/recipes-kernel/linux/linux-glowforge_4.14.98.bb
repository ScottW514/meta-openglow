SUMMARY = "i.MX Linux Kernel"
DESCRIPTION = "i.MX Linux Kernel"

KBRANCH = "imx_4.14.98_2.3.0"
SRCREV = "860ec89b125a6d90729fec87262ebc6596428efe"
LINUX_VERSION = "4.14.98"
LINUX_VERSION_EXTENSION = "-glowforge"

inherit kernel
require recipes-kernel/linux/linux-yocto.inc

DEPENDS += "lzop-native bc-native"

KERNEL_MODULE_AUTOLOAD += "ov5648_camera_slave_mipi"

KMETA = "kernel-meta"

SRC_URI += "git://source.codeaurora.org/external/imx/linux-imx;protocol=https;branch=${KBRANCH} \
            file://git/ \
            file://defconfig \
            file://0001-compiler-attributes-add-support-for-copy-gcc-9.patch \
            file://0002-include-linux-module.h-copy-init-exit-attrs-to-.patch \
            file://1001-Add-DT-Prescaler-to-PWM-Driver.patch \
            file://1002-Add-EPIT-API.patch \
            file://1003-Expose-SDMA-API.patch \
            file://1004-Add-OV5648-Driver.patch \
            file://1005-Add-delay-to-SPI.patch \
            file://1006-Add-ST-LIS2HH12-driver.patch \
            file://1007-Disable-Bus-Freq-Scaling.patch \
           "

DEFAULT_PREFERENCE = "1"
COMPATIBLE_MACHINE = "glowforge"
