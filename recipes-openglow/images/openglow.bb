DESCRIPTION = "OpenGlow Platform"

LICENSE = "GPLv3"

inherit core-image

IMAGE_INSTALL = " \
	packagegroup-core-boot \
	${CORE_IMAGE_EXTRA_INSTALL} \
	i2c-tools \
	u-boot-fw-utils \
	nano \
"

IMAGE_FEATURES += ""
