# Export cross-compiler
source scripts/settings/cross_compilation_settings.sh

# Build linux kernel
make -C work/linux-digilent xilinx_zynq_defconfig
make -C work/linux-digilent -j `nproc`
make -C work/linux-digilent UIMAGE_LOADADDR=0x8000 uImage -j `nproc`
cp work/linux-digilent/arch/arm/boot/uImage SDcard/BOOT/
