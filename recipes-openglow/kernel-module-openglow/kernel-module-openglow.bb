DESCRIPTION = "OpenGlow Kernel Module"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b234ee4d69f5fce4486a80fdaf4a4263"

PV = "0.0.1"

SRC_URI = "file://src/uapi/openglow.h \
           file://src/device_attr.h \
           file://src/io.c \
           file://src/io.h \
           file://src/ledtrig_smooth.c \
           file://src/notifiers.h \
           file://src/openglow.c \
           file://src/pic.c \
           file://src/pic.h \
           file://src/pic_api.c \
           file://src/pic_leds.c \
           file://src/pic_private.h \
           file://src/thermal.c \
           file://src/thermal.h \
           file://src/thermal_private.h \
           file://Makefile \
           file://LICENSE \
"

inherit module

S = "${WORKDIR}"

do_install() {
	install -d ${D}/lib/modules/${KERNEL_VERSION}/extras/
	install -m 0644 *.ko ${D}/lib/modules/${KERNEL_VERSION}/extras/
}
