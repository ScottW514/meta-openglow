# OpenGlow BSP packages for Glowforge
These are the OpenGlow Community Yocto Board Support Packages for Glowforge brand CNC lasers.
The packages cover both the factory and OpenGlow drop-in control boards.

For support, please visit [https://community.openglow.com](https://community.openglow.com).

# Build and Install
Instructions are for a Linux environment supported by Yocto.

To get the BSP you need to have `repo` installed:

## Install the `repo` utility:

```console
~$: mkdir ~/bin
~$: curl http://commondatastorage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
~$: chmod a+x ~/bin/repo
~$: PATH=${PATH}:~/bin
```
Add the following to the end of ```~/.bashrc``` to permanently add ```repo``` to your path:
```console
export PATH=~/bin:$PATH
```
## Download the BSP source:

```console
~$: mkdir openglow-bsp
~$: cd openglow-bsp
~$: repo init -u https://github.com/ScottW514/meta-openglow.git -b master -m glowforge.xml
~$: repo sync
```

At the end of the commands you have all meta packages you need to build the basic firmware images.

## Build the OpenGlow image for factory control boards
First time environment setup:
```console
(in openglow-bsp directory)
~$: MACHINE=glowforge DISTRO=glowforge . setup-environment build
~$: bitbake glowforge-image
```
Subsequent builds:
```console
(in openglow-bsp directory)
~$: . setup-environment build
~$: bitbake glowforge-image
```

## To write the image to a bootable SD card:
```console
(in openglow-bsp/build directory)
~$: cd tmp/deploy/images/glowforge
-$: sudo zcat glowforge-glowforge.wic.gz | dd of=/dev/sdX bs=1M
```
