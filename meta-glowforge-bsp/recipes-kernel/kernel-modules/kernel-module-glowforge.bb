DESCRIPTION = "Glowforge Kernel Module"
LICENSE = "GPLv2"

PV = "0.0.1"

SRC_URI = "git://github.com/ScottW514/kernel-module-glowforge.git;protocol=https"
SRCREV = "0257f5ec6ca54887aae011389acc4345ce3874d4"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b234ee4d69f5fce4486a80fdaf4a4263"

inherit module

S = "${WORKDIR}/git"

do_install() {
	install -d ${D}/lib/modules/${KERNEL_VERSION}/extras/
	install -m 0644 *.ko ${D}/lib/modules/${KERNEL_VERSION}/extras/
}
COMPATIBLE_MACHINE = "glowforge"
