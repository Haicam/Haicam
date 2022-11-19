echo "prepare libhaicam-p2p-onvif.a mac64"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake -DANDROID_ABI=mac64 -DCMAKE_TOOLCHAIN_FILE=../../../build/toolchain/mac-x86_64.toolchain.cmake ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ../../
echo "box_proj mac64/build done"

CGO_ENABLED=1 \
CC=/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-clang \
GOARCH=amd64 \
GOOS=darwin \
go build -tags box -ldflags "-s -w" -buildmode=c-archive -o libhaicam_ext.a *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/mac64/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/mac64/
cp -f libhaicam_ext.h box_proj/build/mac64/
cp -f libhaicam_ext.a box_proj/build/mac64/
rm libhaicam_ext.h
rm libhaicam_ext.a
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "mac64 build done"
