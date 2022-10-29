#https://github.com/ezhomelabs/googletest.git release-1.8.0-mingw

test -d /tmp/googletest && rm -rf /tmp/googletest
cd /tmp
git clone https://github.com/ezhomelabs/googletest.git
cd googletest
git checkout release-1.8.0-mingw

mkdir build
cd build

#linux x86_64
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/linux/x86_64 ..
make -j`nproc` && make install
rm -rf *

#linux i386
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux32.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/linux/i386 ..
make -j`nproc` && make install
rm -rf *

#android armeabi-v7a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/android/armeabi-v7a ..
make -j`nproc` && make install
rm -rf *

#android arm64-v8a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/android/arm64-v8a ..
make -j`nproc` && make install
rm -rf *

#windows x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/windows/x86_64 ..
make -j`nproc` && make install
rm -rf *

#windows i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win32.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/windows/i386 ..
make -j`nproc` && make install
rm -rf *

#mac x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/mac-x86_64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/mac/x86_64 ..
make -j`nproc` && make install
rm -rf *

#ios arm64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/ios-arm64.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/ios/arm64 ..
make -j`nproc` && make install
rm -rf *

#gm8136 armv5
# only support release-1.8.0
git checkout gm8136
cmake -DCMAKE_C_FLAGS='-fPIE' -DCMAKE_CXX_FLAGS='-fPIE' -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-gm8136.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/gm8136/armv5 ..
make -j`nproc` && make install
rm -rf *

#rpi armv7
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-rpi.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/googletest/rpi/armv7 ..
make -j`nproc` && make install
rm -rf *