DESCRIPTION = "OpenGlow Kernel Module"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b234ee4d69f5fce4486a80fdaf4a4263"

PV = "0.0.1"

SRC_URI = "file://include/ \
           file://src/ \
           file://Makefile \
           file://LICENSE \
"

inherit module

S = "${WORKDIR}"

do_install() {
	install -d ${D}/lib/modules/${KERNEL_VERSION}/extras/
	install -m 0644 *.ko ${D}/lib/modules/${KERNEL_VERSION}/extras/
}
