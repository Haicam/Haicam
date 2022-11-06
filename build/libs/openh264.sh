test -d /tmp/openh264 && rm -rf /tmp/openh264
cd /tmp
git clone https://github.com/cisco/openh264.git
cd openh264
git checkout v2.3.1

#linux x86_64
#make CFLAGS='-fPIE' -j`nproc` OS=linux ARCH=x86_64
make -j`nproc` OS=linux ARCH=x86_64 PREFIX=/home/haicam/workspace/libs/openh264/linux/x86_64
make PREFIX=/home/haicam/workspace/libs/openh264/linux/x86_64 install-headers
make PREFIX=/home/haicam/workspace/libs/openh264/linux/x86_64 install-static-lib 
make PREFIX=/home/haicam/workspace/libs/openh264/linux/x86_64 install-static
make clean

#Windows x86_64
make -j`nproc` OS=mingw_nt ARCH=x86_64 PREFIX=/home/haicam/workspace/libs/openh264/windows/x86_64 CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ AR=x86_64-w64-mingw32-ar
make PREFIX=/home/haicam/workspace/libs/openh264/windows/x86_64 install-headers
make PREFIX=/home/haicam/workspace/libs/openh264/windows/x86_64 install-static-lib 
make PREFIX=/home/haicam/workspace/libs/openh264/windows/x86_64 install-static
make clean

#mac x86_64
(
export TOOLCHAIN_PREFIX=/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4
export CC=${TOOLCHAIN_PREFIX}-clang
export CXX=${TOOLCHAIN_PREFIX}-clang++
export AR=${TOOLCHAIN_PREFIX}-ar
export RANLIB=${TOOLCHAIN_PREFIX}-ranlib
export LIBTOOL=${TOOLCHAIN_PREFIX}-libtool
make -j`nproc` OS=darwin ARCH=x86_64 PREFIX=/home/haicam/workspace/libs/openh264/mac/x86_64
make PREFIX=/home/haicam/workspace/libs/openh264/mac/x86_64 install-headers
make PREFIX=/home/haicam/workspace/libs/openh264/mac/x86_64 install-static-lib 
make PREFIX=/home/haicam/workspace/libs/openh264/mac/x86_64 install-static
make clean
)

#rpi armv7
(
export TOOLCHAIN_PREFIX=/home/haicam/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi
export CC=${TOOLCHAIN_PREFIX}-gcc
export CXX ${TOOLCHAIN_PREFIX}-g++
export AR=${TOOLCHAIN_PREFIX}-ar
export RANLIB=${TOOLCHAIN_PREFIX}-ranlib
export LD=${TOOLCHAIN_PREFIX}-ld
make -j`nproc` OS=linux ARCH=arm PREFIX=/home/haicam/workspace/libs/openh264/rpi/armv7
make PREFIX=/home/haicam/workspace/libs/openh264/rpi/armv7 install-headers
make PREFIX=/home/haicam/workspace/libs/openh264/rpi/armv7 install-static-lib 
make PREFIX=/home/haicam/workspace/libs/openh264/rpi/armv7 install-static
make clean
)
