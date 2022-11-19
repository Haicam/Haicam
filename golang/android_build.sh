NDK=/home/haicam/toolchain/android-ndk-r24

echo "prepare libhaicam-p2p.a armeabi-v7a"
cd build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake \
-DCMAKE_TOOLCHAIN_FILE=${NDK}/build/cmake/android.toolchain.cmake \
-DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON  \
-DANDROID_NATIVE_API_LEVEL=21 ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ..


CC=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi21-clang \
GOARCH=arm \
GOARM=7 \
CGO_ENABLED=1 \
CGO_LDFLAGS=-Wl,-soname,libhaicam_ext.so \
GOOS=android \
go build -tags app -ldflags "-s -w" -o libhaicam_ext.so -buildmode=c-shared p2p.go
cp -f build/libhaicam-p2p.a build/armeabi-v7a/
cp -f build/libhaicam-ifaddrs.a build/armeabi-v7a/
cp -f libhaicam_ext.h build/armeabi-v7a/
cp -f libhaicam_ext.so build/armeabi-v7a/
rm libhaicam_ext.h
rm libhaicam_ext.so
rm build/libhaicam-p2p.a
rm build/libhaicam-ifaddrs.a
echo "armeabi-v7a build done"

echo "prepare libhaicam-p2p.a arm64-v8a"
cd build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake \
-DCMAKE_TOOLCHAIN_FILE=${NDK}/build/cmake/android.toolchain.cmake \
-DANDROID_ABI=arm64-v8a \
-DANDROID_NATIVE_API_LEVEL=21 ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ..

CC=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang \
GOARCH=arm64 \
CGO_ENABLED=1 \
CGO_LDFLAGS=-Wl,-soname,libhaicam_ext.so \
GOOS=android \
go build -tags app -ldflags "-s -w" -o libhaicam_ext.so -buildmode=c-shared p2p.go
cp -f build/libhaicam-p2p.a build/arm64-v8a/
cp -f build/libhaicam-ifaddrs.a build/arm64-v8a/
cp -f libhaicam_ext.h build/arm64-v8a/
cp -f libhaicam_ext.so build/arm64-v8a/
rm libhaicam_ext.h
rm libhaicam_ext.so
rm build/libhaicam-p2p.a
rm build/libhaicam-ifaddrs.a
echo "arm64-v8a build done"
