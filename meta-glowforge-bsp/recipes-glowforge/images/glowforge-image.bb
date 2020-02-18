DESCRIPTION = "OpenGlow image for Glowforge"

LICENSE = "GPLv2"

inherit core-image

IMAGE_INSTALL = " \
	packagegroup-base \
	packagegroup-core-boot \
	kernel-devicetree \
	${CORE_IMAGE_EXTRA_INSTALL} \
	curl \
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
	python3-gfhardware \
	python3-gfutilities \
	python3-pillow \
	spitools \
	tcpdump \
	u-boot-env \
	u-boot-fw-utils \
	wpa-supplicant \
	wlconf \
"

create_data_dir() {
	mkdir -p ${IMAGE_ROOTFS}/data
}
IMAGE_PREPROCESS_COMMAND += "create_data_dir; "

link_device_tree() {
	cd ${IMAGE_ROOTFS}/boot
	ln -sf glowforge.dtb imx6dl-glowforge-v20.dtb
}
ROOTFS_POSTPROCESS_COMMAND += "link_device_tree; "
