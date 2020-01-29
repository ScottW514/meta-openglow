FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI += "file://Add-Glowforge-to-Arch-Kconfig.patch \
	file://git/ \
	file://fw_env.config \
"
COMPATIBLE_MACHINE = "glowforge"
