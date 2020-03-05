SUMMARY = "A small collection of utilties to aid in firmware development for the Glowforge laser cutter."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI[md5sum] = "3ae1373b394d76d4e7929fb29ff0b1ce"
SRC_URI[sha256sum] = "031f265fd6b7163bff9946a019b4d641a78b63e68a61ab48c7b40541d628ab50"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS_${PN} += "\
    ${PYTHON_PN}-requests \
    ${PYTHON_PN}-lomond \
    "
