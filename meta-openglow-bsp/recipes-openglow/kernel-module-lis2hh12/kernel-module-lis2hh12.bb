DESCRIPTION = "ST LIS2HH12 Kernel Module"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=7116033ecf2072d9f5f263e09ec7d3e9"

PV = "1.1.1"

SRC_URI = "file://src/ \
           file://Makefile \
           file://LICENSE \
"

inherit module

S = "${WORKDIR}"

do_install() {
	install -d ${D}/lib/modules/${KERNEL_VERSION}/extras/
	install -m 0644 *.ko ${D}/lib/modules/${KERNEL_VERSION}/extras/
}
COMPATIBLE_MACHINE = "openglow_std"
