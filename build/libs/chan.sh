test -d /tmp/chan && rm -rf /tmp/chan
cd /tmp
git clone https://github.com/ezhomelabs/chan.git
cd chan
./autogen.sh 

mkdir build
cd build

#linux x86_64
../configure --prefix=/home/haicam/workspace/libs/chan/linux/x86_64
make CFLAGS='-fPIE' -j`nproc`; make install
rm -rf *

#Windows x86_64
../configure --prefix=/home/haicam/workspace/libs/chan/windows/x86_64 --host=x86_64-w64-mingw32
make CFLAGS='-fPIE' -j`nproc`; make install
rm -rf *

#mac x86_64
(
export TOOLCHAIN_PREFIX=/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4
export CC=${TOOLCHAIN_PREFIX}-clang
export CXX=${TOOLCHAIN_PREFIX}-clang++
../configure --prefix=/home/haicam/workspace/libs/chan/mac/x86_64 --host=x86_64-apple-darwin21.4
make CFLAGS='-fPIE' -j`nproc`; make install
rm -rf *
)

#gm8136 armv5
PATH=/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/:$PATH ../configure --host=arm-unknown-linux-uclibcgnueabi --prefix=/home/haicam/workspace/libs/chan/gm8136/armv5
PATH=/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/:$PATH make CFLAGS='-fPIE' -j`nproc`
PATH=/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/:$PATH make install
rm -rf *

