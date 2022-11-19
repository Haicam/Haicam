echo "prepare libhaicam-p2p-onvif.a rpi32"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake -DANDROID_ABI=rpi32 -DCMAKE_TOOLCHAIN_FILE=../../../build/toolchain/arm-rpi.toolchain.cmake ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ../../
echo "box_proj rpi32/build done"

CGO_ENABLED=1 \
CC=/home/haicam/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc \
GOARCH=arm \
GOARM=7 \
GOOS=linux \
go build -tags box -ldflags "-s -w" -buildmode=c-archive -o libhaicam_ext.a *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/rpi32/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/rpi32/
cp -f libhaicam_ext.h box_proj/build/rpi32/
cp -f libhaicam_ext.a box_proj/build/rpi32/
rm libhaicam_ext.h
rm libhaicam_ext.a
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "rpi32 build done"