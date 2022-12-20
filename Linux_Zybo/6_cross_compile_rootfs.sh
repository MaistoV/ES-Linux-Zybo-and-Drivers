# Export cross-compiler
source work/scripts/settings/cross_compilation_settings.sh

# Build rootfs
cp work/configs/buildroot_config work/buildroot/.config
make -C work/buildroot -j `nproc`
cp work/buildroot/output/images/rootfs.tar SDcard/rootfs/
