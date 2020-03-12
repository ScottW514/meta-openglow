SUMMARY = "A small collection of utilties to aid in firmware development for the Glowforge laser cutter."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI[md5sum] = "ef3e3170015d3e036f6133f626c48f47"
SRC_URI[sha256sum] = "3d4f326dd65969d5ea23183248a70a32c636dd208dc2819fc97e4c2bf2553715"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS_${PN} += "\
    ${PYTHON_PN}-requests \
    ${PYTHON_PN}-lomond \
    "
