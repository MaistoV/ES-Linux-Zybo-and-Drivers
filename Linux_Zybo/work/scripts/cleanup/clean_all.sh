# Custom device tree
rm -rf work/custom_dt
# Linux kernel
make -C work/linux-digilent clean
# U-Boot
make -C work/u-boot-Digilent-Dev clean
# Builroot
make -C work/buildroot clean
# Generated outputs
rm SDcard/*/*

