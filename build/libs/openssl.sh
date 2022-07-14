#https://github.com/openssl/openssl/blob/OpenSSL_1_1_1n/INSTALL

test -d /tmp/openssl && rm -rf /tmp/openssl
cd /tmp
git clone https://github.com/ezhomelabs/openssl
cd openssl
git checkout OpenSSL_1.1.1q

mkdir build
cd build

#linux x86_64
../config --prefix=/home/haicam/workspace/libs/openssl/linux/x86_64 --openssldir=/home/haicam/workspace/libs/openssl/linux/x86_64/ssl no-shared no-engine
make -j`nproc` && make install
rm -rf /home/haicam/workspace/libs/openssl/linux/x86_64/ssl/
rm -rf /home/haicam/workspace/libs/openssl/linux/x86_64/bin/
rm -rf /home/haicam/workspace/libs/openssl/linux/x86_64/share/
rm -rf *

#linux i386
../config --prefix=/home/haicam/workspace/libs/openssl/linux/i386 --openssldir=/home/haicam/workspace/libs/openssl/linux/i386/ssl linux-generic32 no-shared no-engine 386
make -j`nproc` CC="gcc -m32" && make install
rm -rf /home/haicam/workspace/libs/openssl/linux/i386/ssl/
rm -rf /home/haicam/workspace/libs/openssl/linux/i386/bin/
rm -rf /home/haicam/workspace/libs/openssl/linux/i386/share/
rm -rf *

#android armeabi-v7a
(
export ANDROID_NDK_ROOT=/home/haicam/toolchain/android-ndk-r24
export PATH=$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH
../Configure android-arm -D__ANDROID_API__=21 --prefix=/home/haicam/workspace/libs/openssl/android/armeabi-v7a no-shared no-engine
make -j`nproc` 
make install
rm -rf /home/haicam/workspace/libs/openssl/android/armeabi-v7a/ssl/
rm -rf /home/haicam/workspace/libs/openssl/android/armeabi-v7a/bin/
rm -rf /home/haicam/workspace/libs/openssl/android/armeabi-v7a/share/
rm -rf *
)

#android arm64-v8a
(
export ANDROID_NDK_ROOT=/home/haicam/toolchain/android-ndk-r24
export PATH=$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH
../Configure android-arm64 -D__ANDROID_API__=21 --prefix=/home/haicam/workspace/libs/openssl/android/arm64-v8a no-shared no-engine
make -j`nproc` 
make install
rm -rf /home/haicam/workspace/libs/openssl/android/arm64-v8a/ssl/
rm -rf /home/haicam/workspace/libs/openssl/android/arm64-v8a/bin/
rm -rf /home/haicam/workspace/libs/openssl/android/arm64-v8a/share/
rm -rf *
)

#windows x86_64
../config --cross-compile-prefix=x86_64-w64-mingw32- --prefix=/home/haicam/workspace/libs/openssl/windows/x86_64 mingw64 no-shared no-engine no-async
make -j`nproc` && make install
rm -rf /home/haicam/workspace/libs/openssl/windows/x86_64/ssl/
rm -rf /home/haicam/workspace/libs/openssl/windows/x86_64/bin/
rm -rf /home/haicam/workspace/libs/openssl/windows/x86_64/share/
rm -rf *

#windows i386
../config --cross-compile-prefix=i686-w64-mingw32- --prefix=/home/haicam/workspace/libs/openssl/windows/i386 mingw no-shared no-engine no-async
make -j`nproc` && make install
rm -rf /home/haicam/workspace/libs/openssl/windows/i386/ssl/
rm -rf /home/haicam/workspace/libs/openssl/windows/i386/bin/
rm -rf /home/haicam/workspace/libs/openssl/windows/i386/share/
rm -rf *

#mac x86_64
(
export CC="/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-clang"
export AR="/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-ar"
export RANLIB="/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-ranlib"
../config --prefix=/home/haicam/workspace/libs/openssl/mac/x86_64 darwin64-x86_64 no-shared no-engine no-async no-comp "-isysroot /home/haicam/toolchain/MacOSX12.3/SDK/MacOSX12.3.sdk"
make -j`nproc` && make install
rm -rf /home/haicam/workspace/libs/openssl/mac/x86_64/ssl/
rm -rf /home/haicam/workspace/libs/openssl/mac/x86_64/bin/
rm -rf /home/haicam/workspace/libs/openssl/mac/x86_64/share/
rm -rf *
)

#ios arm64
(
export CC="/home/haicam/toolchain/iPhoneOS15.4/bin/arm-apple-darwin11-clang"
export AR="/home/haicam/toolchain/iPhoneOS15.4/bin/arm-apple-darwin11-ar"
export RANLIB="/home/haicam/toolchain/iPhoneOS15.4/bin/arm-apple-darwin11-ranlib"
../config --prefix=/home/haicam/workspace/libs/openssl/ios/arm64 ios64-cross no-shared no-engine no-async no-comp "-isysroot /home/haicam/toolchain/iPhoneOS15.4/SDK/iPhoneOS15.4.sdk"
make -j`nproc` && make install
rm -rf /home/haicam/workspace/libs/openssl/ios/arm64/ssl/
rm -rf /home/haicam/workspace/libs/openssl/ios/arm64/bin/
rm -rf /home/haicam/workspace/libs/openssl/ios/arm64/share/
rm -rf *
)

#gm8136 armv5
../config --cross-compile-prefix=/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/arm-unknown-linux-uclibcgnueabi- --prefix=/home/haicam/workspace/libs/openssl/gm8136/armv5 --openssldir=/home/haicam/workspace/libs/openssl/gm8136/armv5/ssl linux-generic32 no-shared no-engine no-async
make -j`nproc` && make install
rm -rf /home/haicam/workspace/libs/openssl/gm8136/armv5/ssl/
rm -rf /home/haicam/workspace/libs/openssl/gm8136/armv5/bin/
rm -rf /home/haicam/workspace/libs/openssl/gm8136/armv5/share/
rm -rf *

#rpi armv7
../config --cross-compile-prefix=/home/haicam/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi- --prefix=/home/haicam/workspace/libs/openssl/rpi/armv7 --openssldir=/home/haicam/workspace/libs/openssl/rpi/armv7/ssl linux-generic32 no-shared no-engine no-async
make -j`nproc` && make install
rm -rf /home/haicam/workspace/libs/openssl/rpi/armv7/ssl/
rm -rf /home/haicam/workspace/libs/openssl/rpi/armv7/bin/
rm -rf /home/haicam/workspace/libs/openssl/rpi/armv7/share/
rm -rf *