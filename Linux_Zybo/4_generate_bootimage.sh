# Export Xilinx tools to PATH
source work/scripts/settings/xilinx_settings.sh
if [ ! -e work/hw/design.bit ]; 
then 
    echo "ERROR: No bitstream found, please place one in the work/hw/design.bit file"
else
    # Run bootgen from xsct
    # xsct work/scripts/tcl/bootgen.tcl
    # Run bootgen directly
    bootgen -image work/configs/BOOT.bif -arch zynq -w -o SDcard/BOOT/BOOT.BIN -log trace
fi
