SUMMARY = "A small collection of utilties to aid in firmware development for the Glowforge laser cutter."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI[md5sum] = "28aa3b0900492906d4796c54382bb0b7"
SRC_URI[sha256sum] = "fdc24919dc7044d3e09e48541810a2108ed3afac06ff772966699376a72d3112"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS_${PN} += "\
    ${PYTHON_PN}-requests \
    ${PYTHON_PN}-lomond \
    "
