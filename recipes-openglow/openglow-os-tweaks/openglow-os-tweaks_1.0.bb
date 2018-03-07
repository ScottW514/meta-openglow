SUMMARY = "OpenGlow Tweaks to the operating system"
HOMEPAGE = "https://www.openglow.org/"

AUTHOR = "Scott Wiederhold"
SECTION = "base"
LICENSE = "GPLv3"
LIC_FILES_CHKSUM = "file://LICENSE;md5=047e11245628658f30df9a91729a1a6b"

SRC_URI = " \
    file://openglow-profile.sh \
"

do_install() {
    install -d ${D}${sysconfdir}/profile.d/
    install -m 0755 ${WORKDIR}/openglow-profile.sh ${D}${sysconfdir}/profile.d/
}
