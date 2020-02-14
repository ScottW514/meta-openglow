FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
  file://01-Fix-includes.patch \
"
COMPATIBLE_MACHINE = "glowforge"
