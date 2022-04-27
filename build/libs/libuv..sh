#https://github.com/Haicam/libuv v1.44.1

test -d /tmp/libuv && rm -rf /tmp/libuv
cd /tmp
git clone https://github.com/Haicam/libuv
cd libuv
git checkout v1.44.1

mkdir build
cd build

#linux x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/linux/x86_64 ..
make -j`nproc` && make install
rm -rf *

#linux i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux32.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/libuv/linux/i386 ..
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