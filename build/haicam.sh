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