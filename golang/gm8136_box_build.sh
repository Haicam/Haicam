echo "prepare libhaicam-p2p-onvif.a gm8136"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake -DANDROID_ABI=gm8136 -DCMAKE_TOOLCHAIN_FILE=../../../build/toolchain/arm-gm8136.toolchain.cmake ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ../../
echo "box_proj gm8136/build done"

CGO_ENABLED=1 \
CC=/usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/arm-unknown-linux-uclibcgnueabi-gcc \
GOARCH=arm \
GOARM=7 \
GOOS=linux \
go build -tags box -ldflags "-s -w" -buildmode=c-archive -o libhaicam_ext.a *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/gm8136/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/gm8136/
cp -f libhaicam_ext.h box_proj/build/gm8136/
cp -f libhaicam_ext.a box_proj/build/gm8136/
rm libhaicam_ext.h
rm libhaicam_ext.a
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "gm8136 build done"
