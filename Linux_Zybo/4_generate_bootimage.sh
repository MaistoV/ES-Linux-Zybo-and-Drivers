# Export Xilinx tools to PATH
source scripts/settings/xilinx_settings.sh
if [ ! -e hw/design.bit ]; 
then 
    echo "ERROR: No bitstream found, please place one in the hw/design.bit file"
else
    # Run bootgen from xsct
    # xsct scripts/tcl/bootgen.tcl
    # Run bootgen directly
    bootgen -image configs/BOOT.bif -arch zynq -w -o SDcard/BOOT/BOOT.BIN -log trace
fi
