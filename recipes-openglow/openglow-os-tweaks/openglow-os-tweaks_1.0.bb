SUMMARY = "OpenGlow Tweaks to the operating system"
HOMEPAGE = "https://www.openglow.org/"

AUTHOR = "Scott Wiederhold"
SECTION = "base"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=7116033ecf2072d9f5f263e09ec7d3e9"

SRC_URI = " \
    file://openglow-profile.sh \
"

do_install() {
    install -d ${D}${sysconfdir}/profile.d/
    install -m 0755 ${WORKDIR}/openglow-profile.sh ${D}${sysconfdir}/profile.d/
}
