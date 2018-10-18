DESCRIPTION = "OpenGlow Platform"

LICENSE = "GPLv2"

inherit core-image

IMAGE_INSTALL = " \
	packagegroup-base \
	packagegroup-core-boot \
	${CORE_IMAGE_EXTRA_INSTALL} \
	cairo \
	glibc-utils \
	gstreamer1.0 \
	gstreamer1.0-plugins-bad \
	gstreamer1.0-plugins-base \
	gstreamer1.0-plugins-good \
	gstreamer1.0-plugins-imx \
	i2c-tools \
	localedef \
	nano \
	ntp \
	openglow-os-tweaks \
	openssh \
	openssh-sftp \
	openssh-sftp-server \
	pango \
	python \
	python-six \
	python-typing \
	python3 \
	python3-evdev \
	python3-flask \
	python3-jsonschema \
	python3-gfutilities \
	python3-pip \
	python3-pyyaml \
	python3-setuptools \
	python3-smbus \
	python3-spidev \
	rng-tools \
	rt-tests \
	stress-ng \
	u-boot-fw-utils \
	wpa-supplicant \
	xenomai-mercury \
"

# Require for Hardware Diagnostics Tool
# Will be separated into its own recipe when finished.
# python3-flask, python3-pyyaml, python3-requests, python-six, python-typing, python3-jsonschema

IMAGE_FEATURES += " \
	ssh-server-openssh \
	tools-debug \
 "
