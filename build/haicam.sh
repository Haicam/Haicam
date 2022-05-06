test -d ~/workspace/build/output/linux/x86_64/generic || mkdir -p ~/workspace/build/output/linux/x86_64/generic
cd ~/workspace/build/output/linux/x86_64/generic

#linux x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake -DHAICAM_PLATFORM=linux -DHAICAM_ARCH=x86_64 -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/linux/i386/generic || mkdir -p ~/workspace/build/output/linux/i386/generic
cd ~/workspace/build/output/linux/i386/generic

#linux i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux32.toolchain.cmake -DHAICAM_PLATFORM=linux -DHAICAM_ARCH=i386 -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/android/armeabi-v7a/generic || mkdir -p ~/workspace/build/output/android/armeabi-v7a/generic
cd ~/workspace/build/output/android/armeabi-v7a/generic

#android armeabi-v7a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=21 \
    -DHAICAM_PLATFORM=android -DHAICAM_ARCH=armeabi-v7a -DHAICAM_TARGET=generic \
    /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/android/arm64-v8a/generic || mkdir -p ~/workspace/build/output/android/arm64-v8a/generic
cd ~/workspace/build/output/android/arm64-v8a/generic

#android arm64-v8a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a  -DANDROID_NATIVE_API_LEVEL=21 \
    -DHAICAM_PLATFORM=android -DHAICAM_ARCH=arm64-v8a -DHAICAM_TARGET=generic \
    /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/windows/x86_64/generic || mkdir -p ~/workspace/build/output/windows/x86_64/generic
cd ~/workspace/build/output/windows/x86_64/generic

#windows x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win64.toolchain.cmake -DHAICAM_PLATFORM=windows -DHAICAM_ARCH=x86_64 -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/windows/i386/generic || mkdir -p ~/workspace/build/output/windows/i386/generic
cd ~/workspace/build/output/windows/i386/generic

#windows i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win32.toolchain.cmake -DHAICAM_PLATFORM=windows -DHAICAM_ARCH=i386 -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/mac/x86_64/generic || mkdir -p ~/workspace/build/output/mac/x86_64/generic
cd ~/workspace/build/output/mac/x86_64/generic

#mac x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/mac-x86_64.toolchain.cmake -DHAICAM_PLATFORM=mac -DHAICAM_ARCH=x86_64  -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/ios/arm64/generic || mkdir -p ~/workspace/build/output/ios/arm64/generic
cd ~/workspace/build/output/ios/arm64/generic

#ios arm64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/ios-arm64.toolchain.cmake -DHAICAM_PLATFORM=ios -DHAICAM_ARCH=arm64  -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/gm8136/armv5/generic || mkdir -p ~/workspace/build/output/gm8136/armv5/generic
cd ~/workspace/build/output/gm8136/armv5/generic

#gm8136 armv5
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-gm8136.toolchain.cmake -DHAICAM_PLATFORM=gm8136 -DHAICAM_ARCH=armv5 -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/rpi/armv7/generic || mkdir -p ~/workspace/build/output/rpi/armv7/generic
cd ~/workspace/build/output/rpi/armv7/generic

#rpi armv7
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-rpi.toolchain.cmake -DHAICAM_PLATFORM=rpi -DHAICAM_ARCH=armv7 -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace