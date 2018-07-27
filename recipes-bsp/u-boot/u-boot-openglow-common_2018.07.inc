FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += "file://Add-Board-to-Arch-Kconfig.patch \
	file://git/configs/ \
	file://git/include/configs/ \
	file://git/board/openglow/openglow_std/ \
	file://fw_env.config \
"
