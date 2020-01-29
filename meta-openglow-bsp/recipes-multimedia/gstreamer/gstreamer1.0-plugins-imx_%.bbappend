FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://00_imx_gst_add_bggr_support.patch \
"
COMPATIBLE_MACHINE = "openglow_std"
