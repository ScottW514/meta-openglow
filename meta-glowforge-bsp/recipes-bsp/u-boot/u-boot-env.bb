SUMMARY = "U-Boot Environment File"
DESCRIPTION = "Sets U-Boot environment for proper OpenGlow boot on Glowforge factory hardware"

LICENSE = "GPLv2"
LICENSE_PATH = "${S}"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "file://uEnv.txt"

do_install() {
	install -d ${D}/boot
  install -Dm 0644 ${WORKDIR}/uEnv.txt ${D}/boot/uEnv.txt
}

FILES_${PN} = " \
  /boot/uEnv.txt \
"
