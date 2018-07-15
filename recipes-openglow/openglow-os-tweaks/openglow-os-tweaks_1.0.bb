SUMMARY = "OpenGlow Tweaks to the operating system"
HOMEPAGE = "https://www.openglow.org/"

AUTHOR = "Scott Wiederhold"
SECTION = "base"
LICENSE = "GPLv2"
LICENSE_PATH = "${S}"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI = " \
    file://openglow-profile.sh \
"

do_install() {
    install -d ${D}${sysconfdir}/profile.d/
    install -m 0755 ${WORKDIR}/openglow-profile.sh ${D}${sysconfdir}/profile.d/
}
