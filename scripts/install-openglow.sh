#!/bin/sh
# (C) Copyright 2020
# Scott Wiederhold, s.e.wiederhold@gmail.com
# https://community.openglow.org
# SPDX-License-Identifier:    MIT
#
# This program makes a partition on the built-in eMMC flash storage on
# the factory Glowforge, and installs the latest copy of OpenGlow on it.

LIGHTRED="\033[1;31m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[1;33m"
BRIGHT="\033[1;39m"
RESET="\033[0m"
ASTERISK="${BRIGHT}*${RESET}"

stop_gf_services () {
  echo -n "${ASTERISK}Stopping Glowforge services"
  sv stop /sv/glowforge 2>&1 >/dev/null; echo -n "."
  sv stop /sv/glowforge/log 2>&1 >/dev/null; echo -n "."
  sv stop /sv/glowforge-datalogger 2>&1 >/dev/null; echo -n "."
  sv stop /sv/glowforge-datalogger/log 2>&1 >/dev/null; echo -n "."
  sv stop /sv/glowforge-updater 2>&1 >/dev/null; echo -n "."
  sv stop /sv/glowforge-updater/log 2>&1 >/dev/null; echo -n "."
  sv stop /sv/bugeggs 2>&1 >/dev/null; echo -n "."
  sv stop /sv/bugeggs/log 2>&1 >/dev/null; echo "."
}

echo
echo -e "${LIGHTRED} ✺┈┈┈┈┈┈${RESET}"
echo -e "${BRIGHT}Open${RESET}Glow  Installation Tool"
echo

if [ -d "/factory" ]; then
  echo -e "${YELLOW}!! This script must be ran from the factory firmware !!${RESET}"
  exit 2
fi

if [ -d "/ogtmp" ]; then
  # We're resuming after reboot
  echo -e "${BRIGHT}Starting Stage 2:${RESET}"

  echo 0 > /proc/sys/kernel/printk
  stop_gf_services

  echo -n -e "${ASTERISK}Restoring /data files"
  mount -o remount,rw /
  rm -rf /data/etc; echo -n "."
  mv /ogtmp/etc /data/; echo -n "."
  mv /ogtmp/data_pipe.bin /data/; echo -n "."
  mv /ogtmp/dropbear_rsa_host_key /data/; echo -n "."
  rm -rf /ogtmp; echo "."

  echo -n -e "${ASTERISK}Restarting network services."
  /etc/init.d/networking restart 2>&1 >/dev/null
  for VAR in 1 2 3 4 5; do
    sleep 2
    echo -n "."
  done
  echo

  echo -e "${ASTERISK}Downloading latest OpenGlow image:"
  curl -L https://github.com/ScottW514/meta-openglow/releases/latest/download/glowforge-image-glowforge.wic.gz --output /data/glowforge-image-glowforge.wic.gz
  curl -L https://raw.githubusercontent.com/ScottW514/meta-openglow/master/scripts/ogboot --output /data/ogboot
  chmod +x /data/ogboot
  echo

  echo -e "${ASTERISK}Writing OpenGlow image to flash:"
  zcat /data/glowforge-image-glowforge.wic.gz | dd of=/dev/mmcblk2p4 skip=2
  mkdir /data/mnt
  mount /dev/mmcblk2p4 /data/mnt
  sed -i 's/mmcblk1p1/mmcblk2p4/g' /data/mnt/boot/uEnv.txt
  umount /data/mnt
  rm -rf /data/mnt
  rm -f /data/glowforge-image-glowforge.wic.gz

  echo 5 > /proc/sys/kernel/printk

  echo -e "${BRIGHT}Stage 2 Complete!${RESET}"
  echo
  read -n1 -p "Press any key to reboot into OpenGlow..." continue
  /data/ogboot -e4
else
  # We're starting fresh
  echo -e "${LIGHTRED}!!!!!!!!!!!!!!!!     WARNING     !!!!!!!!!!!!!!!!${RESET}"
  echo -e "${YELLOW}         THIS IS EXPERIMENTAL SOFTWARE!${RESET}"
  echo -e "The installation and/or use of this software may"
  echo -e "result in damage to your device and/or property,"
  echo -e "loss of warranty, and severe bodily injury and/or"
  echo -e "death. This software is not affiliated with or"
  echo -e "endorsed by Glowforge. ${BRIGHT}USE IT AT YOUR OWN RISK!${RESET}"
  echo -e "${LIGHTRED}!!!!!!!!!!!!!!!!     WARNING     !!!!!!!!!!!!!!!!${RESET}"
  echo
  read -p "Are you sure you want to continue [N/y]? " continue
  echo
  if [ "$continue" != "y" ]; then
    echo "Wise choice.  Exiting without changes."
    exit 0
  fi
  echo -e "${BRIGHT}Starting Stage 1:${RESET}"

  echo 0 > /proc/sys/kernel/printk
  stop_gf_services

  echo -n -e "${ASTERISK}Backing up critical files on /data partition"
  mount -o remount,rw /; echo -n "."
  mkdir /ogtmp; echo -n "."
  cp -R /data/etc /ogtmp/; echo -n "."
  cp /data/data_pipe.bin /ogtmp/; echo -n "."
  cp /data/dropbear_rsa_host_key /ogtmp/; echo "."

  echo -e "${ASTERISK}Creating partitions (ignore fdisk warning):"
  umount -l /dev/mmcblk2p3
  sed -e 's/\s*\([\+0-9a-zA-Z]*\).*/\1/' << EOF | fdisk /dev/mmcblk2 2>&1 >/dev/null
    o # Removing existing partitions
    n # New partition
    p # Primary partition
    1 # Creating new /data partition
    129 # Start at the end of partition 2
    6528 # 2GB /data parttion
    n # New partition
    p # Primary partition
    2 # Creating new /data partition
    6529 # Start at the end of partition 2
    12928 # 2GB /data parttion
    n # New partition
    p # Primary partition
    3 # Creating new /data partition
    12929 # Start at the end of partition 2
    +2048M # 2GB /data parttion
    n # New partition for OpenGlow
    p # Primary partition (Will automatically select 4)
    75430 # Start at the end of partition 3
      # default, Use remaining space
    w # write the partition table
EOF

  # We need to clear the original partition of filesystem data
  # This way the Glowforge will properly recreate the /data partition
  # on the next reboot.
  dd if=/dev/zero of=/dev/mmcblk2p3 bs=4096 count=4096 2>&1 >/dev/null

  echo -e "${BRIGHT}Completed Stage 1${RESET}"
  echo
  echo -e "The device needs to reboot."
  echo -e "After the device has rebooted, run this script"
  echo -e "to continue the installation process."
  echo
  echo 5 > /proc/sys/kernel/printk
  read -n1 -p "Press any key to reboot..." continue
  echo
  echo "Rebooting..."
  reboot
fi

echo
exit 0
