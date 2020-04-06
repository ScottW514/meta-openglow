DESCRIPTION = "OpenGlow image for Glowforge"

LICENSE = "GPLv2"

inherit core-image

IMAGE_INSTALL = " \
	packagegroup-base \
	packagegroup-core-boot \
	kernel-devicetree \
	${CORE_IMAGE_EXTRA_INSTALL} \
	curl \
	e2fsprogs \
	e2fsprogs-resize2fs \
	glibc-utils \
	glowforge-watchdog \
	i2c-tools \
	localedef \
	nano \
  ntp \
	openssh \
	openssh-sftp \
	openssh-sftp-server \
	procps \
	python3 \
	python3-gfhardware \
	python3-gfutilities \
	python3-spidev \
	spitools \
	tcpdump \
	u-boot-env \
	u-boot-fw-utils \
	wpa-supplicant \
	wlconf \
"

create_dirs() {
	mkdir -p ${IMAGE_ROOTFS}/data
	mkdir -p ${IMAGE_ROOTFS}/factory
	mkdir -p ${IMAGE_ROOTFS}/factory/img1
	mkdir -p ${IMAGE_ROOTFS}/factory/img2
}
IMAGE_PREPROCESS_COMMAND += "create_dirs; "

link_device_tree() {
	cd ${IMAGE_ROOTFS}/boot
	ln -sf glowforge.dtb imx6dl-glowforge-v20.dtb
}
ROOTFS_POSTPROCESS_COMMAND += "link_device_tree; "
