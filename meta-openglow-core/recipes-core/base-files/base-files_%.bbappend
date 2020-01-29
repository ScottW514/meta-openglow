FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"
do_install_basefilesissue () {
	install -m 644 ${WORKDIR}/issue*  ${D}${sysconfdir}
}
