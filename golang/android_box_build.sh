NDK=/home/haicam/toolchain/android-ndk-r24

echo "prepare libhaicam-p2p-onvif.a armeabi-v7a"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake \
-DCMAKE_TOOLCHAIN_FILE=${NDK}/build/cmake/android.toolchain.cmake \
-DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON  \
-DANDROID_NATIVE_API_LEVEL=21 ..
make
cd ../../
echo "box_proj/build armeabi-v7a done"

CC=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi21-clang \
GOARCH=arm \
GOARM=7 \
CGO_ENABLED=1 \
CGO_LDFLAGS=-Wl,-soname,libhaicam_ext.so \
GOOS=android \
go build -tags box -ldflags "-s -w" -o libhaicam_ext.so -buildmode=c-shared *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/armeabi-v7a/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/armeabi-v7a/
cp -f libhaicam_ext.h box_proj/build/armeabi-v7a/
cp -f libhaicam_ext.so box_proj/build/armeabi-v7a/
rm libhaicam_ext.h
rm libhaicam_ext.so
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "armeabi-v7a build done"

echo "prepare libhaicam-p2p.a arm64-v8a"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake \
-DCMAKE_TOOLCHAIN_FILE=${NDK}/build/cmake/android.toolchain.cmake \
-DANDROID_ABI=arm64-v8a \
-DANDROID_NATIVE_API_LEVEL=21 ..
make
cd ../../
echo "box_proj/build arm64-v8a done"

CC=$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang \
GOARCH=arm64 \
CGO_ENABLED=1 \
CGO_LDFLAGS=-Wl,-soname,libhaicam_ext.so \
GOOS=android \
go build -tags box -ldflags "-s -w" -o libhaicam_ext.so -buildmode=c-shared *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/arm64-v8a/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/arm64-v8a/
cp -f libhaicam_ext.h box_proj/build/arm64-v8a/
cp -f libhaicam_ext.so box_proj/build/arm64-v8a/
rm libhaicam_ext.h
rm libhaicam_ext.so
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "arm64-v8a build done"
