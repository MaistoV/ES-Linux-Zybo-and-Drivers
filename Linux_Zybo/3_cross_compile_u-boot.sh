# Export cross-compiler
source scripts/settings/cross_compilation_settings.sh

# Build u-boot
patch work/u-boot-Digilent-Dev/include/configs/zynq_zybo.h patches/zynq_zybo.h.patch
make -C work/u-boot-Digilent-Dev zynq_zybo_config
make -C work/u-boot-Digilent-Dev -j `nproc` 
# Rename image
mv work/u-boot-Digilent-Dev/u-boot work/u-boot-Digilent-Dev/u-boot.elf