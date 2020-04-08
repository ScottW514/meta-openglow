SUMMARY = "Glowforge Web UI Client Daemon"
DESCRIPTION = "Connects to the GFUI"

LICENSE = "MIT"
LICENSE_PATH = "${S}"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
  file://gfui-client.py \
  file://gfui-client.init \
  file://gfui-client.conf.sample \
"
S = "${WORKDIR}"

inherit python3native
inherit update-rc.d
INITSCRIPT_PACKAGES = "${PN}"
INITSCRIPT_NAME_${PN} = "gfui-client"
INITSCRIPT_PARAMS_${PN} = "start 99 1 2 3 4 5 . stop 01 0 6 ."

do_install_append() {
  install -d ${D}${sysconfdir}
  install -d ${D}${sbindir}
  install -d ${D}${INIT_D_DIR}
  install -Dm 0600 ${WORKDIR}/gfui-client.conf.sample ${D}/${sysconfdir}/gfui-client.conf.sample
  install -Dm 0755 ${WORKDIR}/gfui-client.py ${D}/${sbindir}/gfui-client.py
  install -Dm 0755 ${WORKDIR}/gfui-client.init ${D}${INIT_D_DIR}/gfui-client
}

FILES_${PN} = " \
  ${sysconfdir}/gfui-client.conf.sample \
  ${sbindir}/gfui-client.py \
  ${INIT_D_DIR}/gfui-client \
"

RDEPENDS_${PN} += "python3-core python3-gfhardware python3-gfutilities python3-daemonize"
