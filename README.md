# OpenGlow Yocto Board Support Packages (BSP) for Glowforge
This repository contains the board support packages needed to build opensource firmware for Glowforge brand CNC lasers.  

Layers:

* `meta-glowforge-bsp`: machine `glowforge`: the factory NXP i.MX6 control board (kernel, device tree, U-Boot, board recipes).
* `meta-openglow-core`: distro-neutral recipes shared by the images built on it (Glowforge service utilities, base-files, networking).

Both target the Yocto `scarthgap` release and are consumed by the [ForgeFIRM](https://github.com/openglow-org/forgefirm) build.

* [Community Support](https://community.openglow.org)
* [ForgeFIRM Installable Firmware](https://github.com/openglow-org/forgefirm)  

Everything the project knows is on the documentation site,
<https://docs.forgefirm.org/>; the kernel, the device tree, the configuration
fragment and the reserved memory pool are on
[Image and BSP](https://docs.forgefirm.org/technical/forgefirm/image-and-bsp/).

## Contributing

[AGENTS.md](AGENTS.md) carries the rules for this repository and for the
project: safety ordering, proof before done, the push order, and the writing
rules. They apply to human contributors too.

This project is for experimental purposes only, and is not supported or endorsed by Glowforge.
