The xsa/bit files in this directory have been generated with Vivado 2021.2 and might not be compatible with other installations.

You should generate your own bitstream!

1. Enable UART1 (no Modem Signals)
vivado gui: Block Design > PS config > Peripheral I/O config > UART1
vivado tcl: set_property -dict [list CONFIG.PCW_UART1_PERIPHERAL_ENABLE {1}] [get_bd_cells processing_system7_0]
result pcw.dtsi &uart1 { ... status = "okay"; cts-override; ...}

2. set peripheral frequency to 50MHz
vivado gui: Block Design > PS config > Cock Configuration > Input Frequency (MHz) = 50
vivado tcl: set_property -dict [list CONFIG.PCW_CRYSTAL_PERIPHERAL_FREQMHZ {50}] [get_bd_cells processing_system7_0]
result pcw.dtsi: ps-clk-frequency = <0x2faf080>;

3. enable SDcard port 0 (SD0)
vivado gui: Block Design > PS config > Peripheral I/O config > SD0 > Card Detect, Write Protect
vivado tcl: set_property -dict [list CONFIG.PCW_SD0_PERIPHERAL_ENABLE {1} CONFIG.PCW_SD0_GRP_CD_ENABLE {1} CONFIG.PCW_SD0_GRP_WP_ENABLE {1} CONFIG.PCW_SD0_GRP_POW_ENABLE {0} CONFIG.PCW_SD1_PERIPHERAL_ENABLE {0}] [get_bd_cells processing_system7_0]


4. the interrupt signal output by the custom IP must be marked as interrupt:
    * Vivado GUI: Package IP > Ports and Interfaces > <my port> > Right Click > Auto Infere Single Bit Interface > Interrupt
    * Vivado tcl: ipx::infer_bus_interface <my port> xilinx.com:signal:interrupt_rtl:1.0 [ipx::current_core]

5. In the PL configuration, enable PL interrupts
    * Vivado GUI: Block Design > PL configuration > Interrupts > [Check] Fabric Interrupts > PL-PS Interrupt Ports > [Check] IRQ_F2P
    