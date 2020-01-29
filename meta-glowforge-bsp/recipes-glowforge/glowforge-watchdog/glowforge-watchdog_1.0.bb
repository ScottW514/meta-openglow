SUMMARY = "Watchdog Tickling Daemon"
DESCRIPTION = "Keeps watchdog from rebooting the system."

LICENSE = "GPLv2"
LICENSE_PATH = "${S}"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = " \
  file://glowforge-watchdog.c \
  file://glowforge-watchdog.init \
"
S = "${WORKDIR}"

inherit update-rc.d
INITSCRIPT_PACKAGES = "${PN}"
INITSCRIPT_NAME_${PN} = "glowforge-watchdog"
INITSCRIPT_PARAMS_${PN} = "start 01 1 2 3 4 5 . stop 85 0 6 ."

do_compile() {
         ${CC} glowforge-watchdog.c -o glowforge-watchdog
}

do_install_append() {
	install -d ${D}${sbindir}
  install -d ${D}${INIT_D_DIR}
  install -Dm 0755 ${WORKDIR}/glowforge-watchdog ${D}/${sbindir}/glowforge-watchdog
  install -Dm 0755 ${WORKDIR}/glowforge-watchdog.init ${D}${INIT_D_DIR}/glowforge-watchdog
}

FILES_${PN} = " \
  ${sbindir}/glowforge-watchdog \
  ${INIT_D_DIR}/glowforge-watchdog \
"

INSANE_SKIP_${PN} = "ldflags"
