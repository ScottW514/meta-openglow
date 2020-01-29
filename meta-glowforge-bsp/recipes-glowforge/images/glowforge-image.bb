DESCRIPTION = "OpenGlow image for Glowforge"

LICENSE = "GPLv2"

inherit core-image

IMAGE_INSTALL = " \
	packagegroup-base \
	packagegroup-core-boot \
	kernel-devicetree \
	${CORE_IMAGE_EXTRA_INSTALL} \
	glibc-utils \
	glowforge-watchdog \
	i2c-tools \
	localedef \
	nano \
	openssh \
	openssh-sftp \
	openssh-sftp-server \
	procps \
	python3 \
	python3-spidev \
	spitools \
	u-boot-fw-utils \
	wpa-supplicant \
	wlconf \
"
