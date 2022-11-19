echo "prepare libhaicam-p2p-onvif.a linux32"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake -DANDROID_ABI=linux32 ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ../../
echo "box_proj linux32/build done"

CC="gcc -m32" \
GOARCH=386 \
CGO_ENABLED=1 \
GOOS=linux \
go build -tags box -ldflags "-s -w" -buildmode=c-archive -o libhaicam_ext.a *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/linux32/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/linux32/
cp -f libhaicam_ext.h box_proj/build/linux32/
cp -f libhaicam_ext.a box_proj/build/linux32/
rm libhaicam_ext.h
rm libhaicam_ext.a
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "linux32 build done"