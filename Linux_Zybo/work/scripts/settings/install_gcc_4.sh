# Download
if [ ! -e work/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf ];
then
    wget https://releases.linaro.org/components/toolchain/binaries/4.9-2017.01/arm-linux-gnueabihf/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf.tar.xz \
        -O work/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf.tar.xz
    tar xvf work/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf.tar.xz -C work/
    # Remove archive
    rm work/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf.tar.xz
fi
# Verify version
$(pwd)/work/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc --version
