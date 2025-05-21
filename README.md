# ES-Linux-Zybo-and-Drivers
Course material for Embedded Systems Laboratory on embedded Linux.

Verified environment (May 2024):
* Zynq Zybo-7000 (the board lost support and repos do not explicitly support it anymore)
* Ubuntu 18.04 for software builds
* Vivado 2021.2 for hardware design
* Xilinx SDK 2019.1
    * Only for building the FSBL from sources (not mandatory)

# Docker container
Aligning the necessary libraries and compilers for building the necessary boot components under `Linux_Zybo` is tricky and hard to reproduce.
For convenvience, a `Dockerfile` is provided in `docker/` folder to build a verified docker image based on Ubuntu 18.04 and with the necessary tools installed.

Build and launch the docker image by sourcing the scripts in the folder:
```
$ source docker_build.sh
$ source docker_run.sh
```

## Notes
1. If using the docker image, sourcing the script `0_install_prerequisites.sh` is not necessary.

2. Building the source under `Linux_Drivers` does not require any container, since it uses the buildroot's toolchain.



