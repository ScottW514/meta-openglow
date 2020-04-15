inherit setuptools3

SUMMARY = "Python modules for accessing and controlling Glowforge brand CNC laser hardware."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=934c9462b6bc6ddaa471e69ec7aa7b2b"

SRC_URI = "git://github.com/ScottW514/python3-gfhardware.git;branch=master"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

DEPENDS += "libv4l libjpeg-turbo"
