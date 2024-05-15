# Export cross-compiler
source scripts/settings/cross_compilation_settings.sh

# Build rootfs
cp configs/buildroot_config work/buildroot/.defconfig
make -C work/buildroot defconfig
make -C work/buildroot -j `nproc`
cp work/buildroot/output/images/rootfs.tar SDcard/rootfs/
