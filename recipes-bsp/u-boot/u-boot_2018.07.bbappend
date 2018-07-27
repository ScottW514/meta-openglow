require u-boot-openglow-common_${PV}.inc

DEPENDS = "u-boot-mkimage-native"

UPGRADESCRIPT = "${S}/board/openglow/openglow_std/u-boot_upgrade.txt"

do_mkimage () {
    # allow deploy to use the ${MACHINE} name to simplify things
    if [ ! -d ${S}/board/openglow/${MACHINE} ]; then
        mkdir ${S}/board/openglow/${MACHINE}
    fi
    uboot-mkimage -A arm -O linux -T script -C none -a 0 -e 0 \
                -n "Upgrade Script" -d ${UPGRADESCRIPT} \
                ${S}/board/openglow/${MACHINE}/u-boot_upgrade.scr
}
addtask mkimage after do_compile before do_install

do_imginstall () {
    install -D -m 644 ${S}/board/openglow/${MACHINE}/u-boot_upgrade.scr \
                ${D}/u-boot_upgrade_install.scr
    install -D -m 644 ${D}/u-boot_upgrade_install.scr \
                ${DEPLOYDIR}/u-boot_upgrade-${MACHINE}-${PV}-${PR}.scr

    cd ${DEPLOYDIR}
    rm -f u-boot_upgrade-${MACHINE}.scr
    ln -sf u-boot_upgrade-${MACHINE}-${PV}-${PR}.scr \
                u-boot_upgrade-${MACHINE}.scr
}
addtask imginstall after do_mkimage before do_install
