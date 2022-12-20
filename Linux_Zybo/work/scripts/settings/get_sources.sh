# Device tree
# Must be the same branch (tag) as Xilinx tools'
git clone --depth 1 -b xilinx_v2021.2 https://github.com/Xilinx/device-tree-xlnx.git work/device-tree-xlnx
# Linux kernel
git clone --depth 1 https://github.com/Digilent/linux-digilent.git work/linux-digilent
# U-boot
# Note: requires GCC version < 5
git clone --depth 1 -b master-next https://github.com/DigilentInc/u-boot-Digilent-Dev.git work/u-boot-Digilent-Dev
# Buildroot
git clone --depth 1 https://github.com/buildroot/buildroot.git work/buildroot

