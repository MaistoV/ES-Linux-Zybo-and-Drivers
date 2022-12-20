setws .
app create -name zynq_fsbl -hw work/hw/design.xsa -os standalone -proc ps7_cortexa9_0 -template {Zynq FSBL}
app config -name zynq_fsbl define-compiler-symbols {FSBL_DEBUG_INFO}
app build -name zynq_fsbl