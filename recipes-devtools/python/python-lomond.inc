SUMMARY = "Tranquil WebSockets for Python."

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI[md5sum] = "48a99b6bd83d1825d37bac5df70b57dd"
SRC_URI[sha256sum] = "701a66db761140dee654499675f216ed624c3ede5c6550fe7fb8929bb0e2fc97"

BBCLASSEXTEND = "native nativesdk"

DEPENDS += "${PYTHON_PN}-pytest-runner-native"
