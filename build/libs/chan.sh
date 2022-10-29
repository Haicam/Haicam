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


#gm8136 armv5
PATH=/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/:$PATH ../configure --host=arm-unknown-linux-uclibcgnueabi --prefix=/home/haicam/workspace/libs/chan/gm8136/armv5
PATH=/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/:$PATH make CFLAGS='-fPIE' -j`nproc`
PATH=/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/:$PATH make install
rm -rf *