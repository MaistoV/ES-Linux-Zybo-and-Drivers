# Device tree
# Must be the same branch (tag) as Xilinx tools'
git clone --depth 1 -b xilinx_v2021.2 https://github.com/Xilinx/device-tree-xlnx.git work/device-tree-xlnx
# Linux kernel
git clone --depth 1 https://github.com/Digilent/linux-digilent.git work/linux-digilent
# U-boot
# Note: requires GCC version < 5
git clone --depth 1 -b master-next https://github.com/DigilentInc/u-boot-Digilent-Dev.git work/u-boot-Digilent-Dev
# Buildroot
git clone https://github.com/buildroot/buildroot.git work/buildroot
cd work/buildroot
git checkout feda018834aa59d8b9398604f9c91e08de87943f
cd ../..
