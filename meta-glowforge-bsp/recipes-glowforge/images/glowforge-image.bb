DESCRIPTION = "OpenGlow image for Glowforge"

LICENSE = "GPL-2.0-only"

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
	forgefirm-uenv \
	libubootenv-bin \
	wpa-supplicant \
	wlconf \
"

# Deliberately NOT installed: the watchdog daemon. The hardware watchdog
# is a boot/system watchdog (U-Boot arms it; the pinned imx2_wdt driver
# adopts it and the kernel keeps it fed while /dev/watchdog is unopened).
# A userspace petter adds only one failure mode here: a crashed daemon
# resets the machine mid-job.

# /data is the writable partition (p3); the rootfs mounts read-only
# (recipes-core/base-files/base-files/fstab). The factory-slot mount
# points belong to the dev image alone (forgefirm-image-dev.bb).
create_dirs() {
	mkdir -p ${IMAGE_ROOTFS}/data
}
IMAGE_PREPROCESS_COMMAND += "create_dirs "

# The factory bootloader looks for imx6dl-glowforge-v<major><minor>.dtb and
# then -v<major>.dtb from the board-revision fuse; both names resolve.
link_device_tree() {
	cd ${IMAGE_ROOTFS}/boot
	ln -sf glowforge.dtb imx6dl-glowforge-v20.dtb
	ln -sf glowforge.dtb imx6dl-glowforge-v2.dtb
}
# No semicolon after a function name: the value is the vardeps of the
# task, split on whitespace, so "name;" would leave the body untracked.
ROOTFS_POSTPROCESS_COMMAND += "link_device_tree "
