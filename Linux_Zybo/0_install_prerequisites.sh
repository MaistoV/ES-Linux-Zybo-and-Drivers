apt-get install -y bc make u-boot-tools libncurses-dev \
    device-tree-compiler gparted git \
    build-essential libssl1.0-dev wget
    
# Download gcc v4.9
source scripts/settings/install_gcc_4.sh

# Clone all the repos
source scripts/settings/get_sources.sh

