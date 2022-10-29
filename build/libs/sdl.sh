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