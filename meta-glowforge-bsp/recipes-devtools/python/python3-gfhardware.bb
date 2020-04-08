inherit setuptools3

SUMMARY = "Python modules for accessing and controlling Glowforge brand CNC laser hardware."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "git://github.com/ScottW514/python3-gfhardware.git;branch=master"
SRCREV = "eeba1c46862346c48a69c66d68d5010af8ead882"

S = "${WORKDIR}/git"

DEPENDS += "libv4l libjpeg-turbo"
