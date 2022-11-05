test -d /tmp/zbar-0.10 && rm -rf /tmp/zbar-0.10
cd /tmp
wget https://ixpeering.dl.sourceforge.net/project/zbar/zbar/0.10/zbar-0.10.tar.bz2
tar -xvjf zbar-0.10.tar.bz2
cd zbar-0.10
patch -p1 < ~/workspace/build/libs/patches/libzbar_mingw64.patch

mkdir build
cd build

#linux x86_64
../configure --prefix=/home/haicam/workspace/libs/zbar/linux/x86_64 --disable-shared --disable-video --without-libiconv-prefix --without-xshm --without-xv --without-jpeg --without-imagemagick --without-gtk --without-python --without-qt
make CFLAGS='-fPIE' -j`nproc`; make install
rm -rf *

#Windows x86_64
#apt install win-iconv-mingw-w64-dev
../configure --host=x86_64-w64-mingw32 --prefix=/home/haicam/workspace/libs/zbar/windows/x86_64 --disable-shared --disable-video --without-libiconv-prefix --without-xshm --without-xv --without-jpeg --without-imagemagick --without-gtk --without-python --without-qt
make CFLAGS='-fPIE' -j`nproc`; make install
rm -rf *

#mac x86_64
(
export TOOLCHAIN_PREFIX=/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4
export CC=${TOOLCHAIN_PREFIX}-clang
export CXX=${TOOLCHAIN_PREFIX}-clang++
../configure --host=x86_64-apple-darwin21.4 --prefix=/home/haicam/workspace/libs/zbar/mac/x86_64 --disable-shared --disable-video --without-libiconv-prefix --without-xshm --without-xv --without-jpeg --without-imagemagick --without-gtk --without-python --without-qt
make CFLAGS='-fPIE' -j`nproc`; make install
rm -rf *
)
