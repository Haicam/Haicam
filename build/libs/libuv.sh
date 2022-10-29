#https://github.com/Haicam/libuv 9e59aa1bc8c4d215ea3e05eafec7181747206f67 (branch v1.x)

test -d /tmp/libuv && rm -rf /tmp/libuv
cd /tmp
git clone https://github.com/Haicam/libuv
cd libuv
git checkout 9e59aa1bc8c4d215ea3e05eafec7181747206f67

mkdir build
cd build

#linux x86_64
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/linux/x86_64 ..
make -j`nproc` && make install
rm -rf *

#linux i386
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux32.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/linux/i386 ..
make -j`nproc` && make install
rm -rf *

#android armeabi-v7a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/android/armeabi-v7a ..
make -j`nproc` && make install
rm -rf *

#android arm64-v8a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/android/arm64-v8a ..
make -j`nproc` && make install
rm -rf *

#windows x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/windows/x86_64 ..
make -j`nproc` && make install
rm -rf *

#windows i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win32.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/windows/i386 ..
make -j`nproc` && make install
rm -rf *

#mac x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/mac-x86_64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/mac/x86_64 ..
make -j`nproc` && make install
rm -rf *

#ios arm64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/ios-arm64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/ios/arm64 ..
make -j`nproc` && make install
rm -rf *

#rpi armv7
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-rpi.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/rpi/armv7 ..
make -j`nproc` && make install
rm -rf *

#gm8136 armv5
git checkout gm8136
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-gm8136.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/gm8136/armv5 ..
make -j`nproc` && make install
git checkout 9e59aa1bc8c4d215ea3e05eafec7181747206f67
rm -rf *