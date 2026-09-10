FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"
do_install_basefilesissue () {
	install -m 644 ${WORKDIR}/issue*  ${D}${sysconfdir}
}

# The machine mark, in three files with three escaping rules:
#   issue      the serial-console login banner. busybox getty parses it:
#              a backslash and a percent sign each start an escape, so
#              art writes them doubled. ANSI color works.
#   issue.net  the pre-authentication network banner. Nothing sends it
#              unless a Banner line names it; the ssh client would print
#              a control character in it as an octal escape, so it is
#              plain text.
#   motd       what a login prints. Written out as it is: ANSI color
#              works and nothing is doubled.
# motd rides poky's own file:// entry, which this layer's copy overrides
# (FILESEXTRAPATHS above), and poky's do_install installs it. The
# version line is appended to issue and motd at image time
# (meta-forgefirm, forgefirm-image.bb).

# The prompt rides poky's own profile through profile.d, so the profile's
# terminal-size handling on the serial console stays.
SRC_URI += "file://openglow-prompt.sh"
do_install:append () {
	install -d ${D}${sysconfdir}/profile.d
	install -m 0644 ${WORKDIR}/openglow-prompt.sh ${D}${sysconfdir}/profile.d/openglow-prompt.sh
}
