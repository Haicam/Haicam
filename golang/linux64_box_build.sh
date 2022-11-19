echo "prepare libhaicam-p2p-onvif.a linux64"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake -DANDROID_ABI=linux64 ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ../../
echo "box_proj linux64/build done"

CC=gcc \
GOARCH=amd64 \
CGO_ENABLED=1 \
GOOS=linux \
go build -tags box -ldflags "-s -w" -buildmode=c-archive -o libhaicam_ext.a *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/linux64/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/linux64/
cp -f libhaicam_ext.h box_proj/build/linux64/
cp -f libhaicam_ext.a box_proj/build/linux64/
rm libhaicam_ext.h
rm libhaicam_ext.a
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "linux64 build done"