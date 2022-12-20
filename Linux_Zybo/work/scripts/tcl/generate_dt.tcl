hsi::open_hw_design work/hw/design.xsa
hsi::set_repo_path work/device-tree-xlnx
hsi::create_sw_design device_tree -os device_tree -proc ps7_cortexa9_0
hsi::generate_target -dir work/custom_dt
