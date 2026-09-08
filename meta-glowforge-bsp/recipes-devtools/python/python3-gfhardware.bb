inherit setuptools3

SUMMARY = "Python modules for accessing and controlling Glowforge brand CNC laser hardware."

# gfhardware/src/bayer.c (libdc1394's Bayer decoder) is LGPL-2.1-or-later and
# is compiled into the gfhardware._cam extension alongside the MIT sources.
LICENSE = "MIT & LGPL-2.1-or-later"
LIC_FILES_CHKSUM = "     file://LICENSE;md5=7029670f29302d9352647c6c89614bc2     file://LICENSE.LGPL-2.1;md5=2a4f4fd2128ea2f65047ee63fbca9f68     file://gfhardware/src/bayer.c;beginline=1;endline=23;md5=c9ff59d801d2fd49e79e261521f425a2 "

SRC_URI = "git://github.com/openglow-org/python3-gfhardware.git;protocol=https;branch=master"
# SRCREV lives in the pin file (ForgeFIRM image manifest: *-pin.inc is left
# out of the layer content hash, the component entry identifies the source).
require python3-gfhardware-pin.inc

S = "${WORKDIR}/git"

DEPENDS += "libv4l libjpeg-turbo"

# gfhardware.cam configures the imx-media capture pipeline at runtime by
# shelling out to media-ctl (links/pad formats) and v4l2-ctl (sensor controls).
RDEPENDS:${PN} += "media-ctl v4l-utils"
# gfhardware.machine imports gfutilities at package import.
RDEPENDS:${PN} += "python3-gfutilities"
# The standard-library packages the modules import.
RDEPENDS:${PN} += " \
    python3-core \
    python3-fcntl \
    python3-json \
    python3-logging \
    python3-netclient \
    python3-threading \
"
