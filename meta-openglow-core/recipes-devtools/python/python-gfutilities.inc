SUMMARY = "A small collection of utilties to aid in firmware development for the Glowforge laser cutter."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI[md5sum] = "95f151df6935ee4fc2f8f1807cac9202"
SRC_URI[sha256sum] = "7100da230d3baa31bdaf25fe1223aa8f50432a6b66c67902e7515160a9703bae"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS_${PN} += "\
    ${PYTHON_PN}-requests \
    ${PYTHON_PN}-lomond \
    "
