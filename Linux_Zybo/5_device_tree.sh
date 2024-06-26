# Export Xilinx tools to PATH
source scripts/settings/xilinx_settings.sh

# Device tree
xsct scripts/tcl/generate_dt.tcl 
# Edit the compatible kernel module name
patch work/custom_dt/pl.dtsi < patches/pl.dtsi.patch
# Edit kernel bootargs
patch work/custom_dt/system-top.dts patches/system-top.dts.patch
# Preprocess includes
gcc -I work/custom_dt -E -nostdinc -undef -D__DTS__ -x assembler-with-cpp -o work/custom_dt/system-top.dts.tmp work/custom_dt/system-top.dts
# Compress into device tree blob
dtc -I dts -O dtb -o SDcard/BOOT/devicetree.dtb work/custom_dt/system-top.dts.tmp
