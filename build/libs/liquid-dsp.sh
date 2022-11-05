test -d /tmp/liquid-dsp && rm -rf /tmp/liquid-dsp
cd /tmp
git clone https://github.com/ezhomelabs/liquid-dsp
cd liquid-dsp
./bootstrap.sh

mkdir build
cd build

#linux x86_64
./configure --prefix=/home/haicam/workspace/libs/liquid/linux/x86_64
make CFLAGS='-std=c99 -fPIE -fPIC -D_GNU_SOURCE=1' -j`nproc`;make install
make clean

#Windows x86_64
git checkout v1.4.0
make prefix=/home/haicam/workspace/libs/liquid/windows/x86_64 CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ -f ~/workspace/build/libs/patches/liquid-dsp.makefile.mingw64 -j
make prefix=/home/haicam/workspace/libs/liquid/windows/x86_64 -f ~/workspace/build/libs/patches/liquid-dsp.makefile.mingw64 install
make -f ~/workspace/build/libs/patches/liquid-dsp.makefile.mingw64 -j clean
git checkout master

#Windows i386
git checkout v1.4.0
make prefix=/home/haicam/workspace/libs/liquid/windows/i386 CC=i686-w64-mingw32-gcc CXX=i686-w64-mingw32-g++ -f ~/workspace/build/libs/patches/liquid-dsp.makefile.mingw64 -j
make prefix=/home/haicam/workspace/libs/liquid/windows/i386 -f ~/workspace/build/libs/patches/liquid-dsp.makefile.mingw64 install
make -f ~/workspace/build/libs/patches/liquid-dsp.makefile.mingw64 -j clean
git checkout master

#mac x86_64
(
export TOOLCHAIN_PREFIX=/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4
export CC=${TOOLCHAIN_PREFIX}-clang
export CXX=${TOOLCHAIN_PREFIX}-clang++
export AR=${TOOLCHAIN_PREFIX}-ar
export RANLIB=${TOOLCHAIN_PREFIX}-ranlib
export LIBTOOL=${TOOLCHAIN_PREFIX}-libtool
./configure --host=x86_64-apple-darwin21.4 --prefix=/home/haicam/workspace/libs/liquid/mac/x86_64
sed -i '/rpl_malloc/,+1d' config.h
sed -i '/rpl_realloc/,+1d' config.h
make CFLAGS='-std=c99 -Dac_cv_func_malloc_0_nonnull=yes -Dac_cv_func_realloc_0_nonnull=yes' -j`nproc`; make install
make clean
#rename libliquid.ar to libliquid.a after install
)
