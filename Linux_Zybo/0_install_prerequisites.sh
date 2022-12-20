sudo apt-get install -y make u-boot-tools libncurses-dev \
    device-tree-compiler gparted git \
    build-essential libssl1.0-dev wget
    
# Download gcc v4.9
source work/scripts/settings/install_gcc_4.sh

# Clone all the repos
source work/scripts/settings/get_sources.sh

