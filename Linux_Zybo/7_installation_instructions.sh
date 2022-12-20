echo "After building and packaging are finished, perform the following steps:
    1. Format a micro SD card with the following layout:     
      4MB         unallcated     
      1GB         fat32 formatted, for the BOOT partition     
      remainder   ext4 formatted , for the rootfs partition     
    2. Copy the content of SDcard/BOOT/ into the BOOT partition     
    3. Extract SDcard/rootfs/rootfs.tar into the rootfs partition     
    4. Insert the micro SD card into the Zybo J4 slot     
    5. Move jumper J5 into SD position     
    6. Power-up the Zybo Board     
    7. Connect a terminal (emulator) to the USB port on J11 slot     
    8. Login with username: root"

