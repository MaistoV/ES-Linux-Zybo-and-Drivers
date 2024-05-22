# Build the FSBL locally
# if [ ! -e work/fsbl/fsbl.elf ];
# then
#     # Export Xilinx tools to PATH
#     source scripts/settings/xilinx_settings.sh
#     # Create and build 
#     xsct scripts/tcl/generate_fsbl.tcl
#     # 
#     mkdir -p work/fsbl
#     cp zynq_fsbl/Debug/zynq_fsbl.elf fsbl/fsbl.elf
#     rm -rf zynq_fsbl
# fi

# Alternatively, fetch it from a pre-built release
if [ ! -e work/fsbl/fsbl.elf ];
then
    mkdir -p work/fsbl
    # Download Petalinux BSP
    wget https://github.com/Digilent/Petalinux-Zybo/releases/download/v2017.4-1/Petalinux-Zybo-2017.4-1.bsp \
        -O work/Petalinux-Zybo-2017.4-1.bsp
    # Extract the archive
    tar xvf work/Petalinux-Zybo-2017.4-1.bsp -C work/
    # Fetch the pre-compiled FSBL
    cp work/Zybo/components/plnx_workspace/fsbl/fsbl/Release/fsbl.elf work/fsbl/
    # Remove the extracted directory and archive
    rm -rf work/Zybo work/Petalinux-Zybo-2017.4-1.bsp
fi