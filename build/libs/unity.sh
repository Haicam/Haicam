#https://github.com/ezhomelabs/Unity v2.5.2

test -d /tmp/Unity && rm -rf /tmp/Unity
cd /tmp
git clone https://github.com/ezhomelabs/Unity
cd Unity
git checkout v2.5.2

mkdir build
cd build

#linux x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/linux/x86_64 -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/linux/x86_64 ..
make -j`nproc` && make install
rm -rf *

#linux i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux32.toolchain.cmake -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/linux/i386 -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/linux/i386 ..
make -j`nproc` && make install
rm -rf *

#android armeabi-v7a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/android/armeabi-v7a \
    -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/android/armeabi-v7a ..
make -j`nproc` && make install
rm -rf *

#android arm64-v8a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a  -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/android/arm64-v8a \
    -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/android/arm64-v8a ..
make -j`nproc` && make install
rm -rf *

#windows x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win64.toolchain.cmake -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/windows/x86_64 -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/windows/x86_64 ..
make -j`nproc` && make install
rm -rf *

#windows i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win32.toolchain.cmake -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/windows/i386 -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/windows/i386 ..
make -j`nproc` && make install
rm -rf *

#mac x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/mac-x86_64.toolchain.cmake -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/mac/x86_64 -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/mac/x86_64 ..
make -j`nproc` && make install
rm -rf *

#ios arm64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/ios-arm64.toolchain.cmake -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/ios/arm64 -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/ios/arm64 ..
make -j`nproc` && make install
rm -rf *

#gm8136 armv5
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-gm8136.toolchain.cmake -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/gm8136/armv5 -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/gm8136/armv5 ..
make -j`nproc` && make install
rm -rf *

#rpi armv7
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-rpi.toolchain.cmake -DCMAKE_INSTALL_LIBDIR=/home/haicam/workspace/libs/unity/rpi/armv7 -DCMAKE_INSTALL_INCLUDEDIR=/home/haicam/workspace/libs/unity/rpi/armv7 ..
make -j`nproc` && make install
rm -rf *