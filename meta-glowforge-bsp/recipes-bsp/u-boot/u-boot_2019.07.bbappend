require u-boot-glowforge-common_${PV}.inc

DEPENDS += "u-boot-mkimage-native"
SRC_URI += "file://uEnv.txt"

do_install_append() {
    install -d ${D}/boot
    install ${WORKDIR}/uEnv.txt ${D}/boot/uEnv.txt
    install -d ${DEPLOYDIR}
		install ${D}/boot/uEnv.txt ${DEPLOYDIR}/uEnv.txt
    cd ${DEPLOY_DIR_IMAGE}
    rm -f uEnv.txt
    install -d ${DEPLOY_DIR_IMAGE}
    install ${D}/boot/uEnv.txt ${DEPLOY_DIR_IMAGE}/uEnv.txt
}

FILES_${PN} += " \
  ${DEPLOYDIR}/uEnv.txt \
  ${DEPLOY_DIR_IMAGE}/uEnv.txt \
"
