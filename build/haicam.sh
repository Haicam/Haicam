test -d ~/workspace/build/output/linux/x86_64 || mkdir -p ~/workspace/build/output/linux/x86_64
cd ~/workspace/build/output/linux/x86_64

#linux x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake -DHAICAM_PLATFORM=linux -DHAICAM_TARGET=x86_64 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/linux/i386 || mkdir -p ~/workspace/build/output/linux/i386
cd ~/workspace/build/output/linux/i386

#linux i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux32.toolchain.cmake -DHAICAM_PLATFORM=linux -DHAICAM_TARGET=i386 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/android/armeabi-v7a || mkdir -p ~/workspace/build/output/android/armeabi-v7a
cd ~/workspace/build/output/android/armeabi-v7a

#android armeabi-v7a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON -DANDROID_NATIVE_API_LEVEL=21 \
    -DHAICAM_PLATFORM=android -DHAICAM_TARGET=armeabi-v7a \
    /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/android/arm64-v8a || mkdir -p ~/workspace/build/output/android/arm64-v8a
cd ~/workspace/build/output/android/arm64-v8a

#android arm64-v8a
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/toolchain/android-ndk-r24/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a  -DANDROID_NATIVE_API_LEVEL=21 \
    -DHAICAM_PLATFORM=android -DHAICAM_TARGET=arm64-v8a \
    /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/windows/x86_64 || mkdir -p ~/workspace/build/output/windows/x86_64
cd ~/workspace/build/output/windows/x86_64

#windows x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win64.toolchain.cmake -DHAICAM_PLATFORM=windows -DHAICAM_TARGET=x86_64 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/windows/i386 || mkdir -p ~/workspace/build/output/windows/i386
cd ~/workspace/build/output/windows/i386

#windows i386
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win32.toolchain.cmake -DHAICAM_PLATFORM=windows -DHAICAM_TARGET=i386 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/mac/x86_64 || mkdir -p ~/workspace/build/output/mac/x86_64
cd ~/workspace/build/output/mac/x86_64

#mac x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/mac-x86_64.toolchain.cmake -DHAICAM_PLATFORM=mac -DHAICAM_TARGET=x86_64 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/ios/arm64 || mkdir -p ~/workspace/build/output/ios/arm64
cd ~/workspace/build/output/ios/arm64

#ios arm64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/ios-arm64.toolchain.cmake -DHAICAM_PLATFORM=ios -DHAICAM_TARGET=arm64 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/gm8136/armv5 || mkdir -p ~/workspace/build/output/gm8136/armv5
cd ~/workspace/build/output/gm8136/armv5

#gm8136 armv5
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-gm8136.toolchain.cmake -DHAICAM_PLATFORM=gm8136 -DHAICAM_TARGET=armv5 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace

test -d ~/workspace/build/output/rpi/armv7 || mkdir -p ~/workspace/build/output/rpi/armv7
cd ~/workspace/build/output/rpi/armv7

#rpi armv7
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-rpi.toolchain.cmake -DHAICAM_PLATFORM=rpi -DHAICAM_TARGET=armv7 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace