SUMMARY = "A small collection of utilties to aid in firmware development for the Glowforge laser cutter."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI[md5sum] = "68c3afcc82c73abd1d025c8e4b4b238d"
SRC_URI[sha256sum] = "252304c5dc12975fc307ee02c1614141f0f03aeb392c7510c3113621280eff7c"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS_${PN} += "\
    ${PYTHON_PN}-requests \
    ${PYTHON_PN}-lomond \
    "
