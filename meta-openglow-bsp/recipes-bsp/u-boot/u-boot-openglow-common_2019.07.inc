FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += "file://Add-OpenGlow-to-Arch-Kconfig.patch \
	file://git/configs/ \
	file://git/include/configs/ \
	file://git/board/openglow/openglow_std/ \
	file://fw_env.config \
"
COMPATIBLE_MACHINE = "openglow_std"
