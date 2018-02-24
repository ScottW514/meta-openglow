DESCRIPTION = "OpenGlow Platform"

LICENSE = "GPLv3"

inherit core-image

IMAGE_INSTALL += " \
	packagegroup-core-boot \
	${CORE_IMAGE_EXTRA_INSTALL} \
	nano \
"

IMAGE_FEATURES += ""

