DESCRIPTION = "OpenGlow Platform"

LICENSE = "GPLv2"

inherit core-image

IMAGE_INSTALL = " \
	packagegroup-base \
	packagegroup-core-boot \
	${CORE_IMAGE_EXTRA_INSTALL} \
	glibc-utils \
	i2c-tools \
	localedef \
	nano \
	ntp \
	openglow-os-tweaks \
	openssh \
	openssh-sftp \
	openssh-sftp-server \
	python \
	python-six \
	python-typing \
	python3 \
	python3-flask \
	python3-jsonschema \
	python3-pip \
	python3-pyyaml \
	python3-requests \
	python3-setuptools \
	python3-smbus \
	python3-spidev \
	stress-ng \
	u-boot-fw-utils \
	wpa-supplicant \
"

IMAGE_FEATURES += " ssh-server-openssh "

# Require for Hardware Diagnostics Tool
# Will be separated into its own recipe when finished.
# python3-flask, python3-pyyaml, python3-requests, python-six, python-typing, python3-jsonschema
