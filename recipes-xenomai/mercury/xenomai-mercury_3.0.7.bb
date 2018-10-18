DESCRIPTION = "POSIX and traditional RTOS APIs for porting time-critical applications to Linux-based platforms"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = "\
    https://gitlab.denx.de/Xenomai/xenomai/-/archive/v${PV}/xenomai-v${PV}.tar.gz;name=xenomai \
    file://xenomai.conf \
"
SRC_URI[xenomai.sha256sum] = "4818eae60ea6da409d7900769a5a95744a8e0147a494cf79f2cab57e8b3606b8"

S = "${WORKDIR}/xenomai-v${PV}/"
includedir = "/usr/xenomai/lib"
oldincludedir = "/usr/xenomai/lib"
prefix = "/usr/xenomai"
exec_prefix = "/usr/xenomai"

FILES_${PN} += "/usr/xenomai/demo/"
INSANE_SKIP_${PN} += "dev-deps"

EXTRA_OECONF_append_arm = " --with-core=mercury --enable-smp --enable-pshared"
DEPENDS = "linux-openglow-rt"
COMPATIBLE_MACHINE = "openglow_std"

inherit autotools gettext

do_bootstrap() {
    cd ${S}
    ./scripts/bootstrap
}
addtask do_bootstrap before do_configure

do_install_append() {
    install -d ${D}${sysconfdir}/ld.so.conf.d/
    install -m 0644 ${WORKDIR}/xenomai.conf ${D}${sysconfdir}/ld.so.conf.d/
}
