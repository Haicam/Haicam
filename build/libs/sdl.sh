test -d /tmp/libsdl && rm -rf /tmp/libsdl
cd /tmp
git clone https://github.com/ezhomelabs/SDL.git libsdl
cd libsdl
git checkout release-2.24.1

mkdir build
cd build

#linux x86_64
#apt-get install libasound2-dev libpulse-dev
#configure script find the headers so it includes PulseAudio and ALSA support.
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libsdl/linux/x86_64 ..
make -j`nproc` && make install
rm -rf *

#windows x86_64
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libsdl/windows/x86_64 ..
make -j`nproc` && make install
rm -rf *

#mac x86_64 - build in Mac computer
#brew install cmake
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_INSTALL_PREFIX=`pwd`/mac_sdl_x86_64 ..
make -j && make install
rm -rf *

#rpi armv7
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DSDL_WAYLAND=OFF -DSDL_PULSEAUDIO=OFF -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-rpi.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libsdl/rpi/armv7 ..
make -j && make install
rm -rf *
