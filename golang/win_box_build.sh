echo "prepare libhaicam-p2p-onvif.a win64"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake -DANDROID_ABI=win64 \
-DCMAKE_TOOLCHAIN_FILE=../../../build/toolchain/win64.toolchain.cmake ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ../../
echo "box_proj/build done"

CC=x86_64-w64-mingw32-gcc \
GOARCH=amd64 \
CGO_ENABLED=1 \
GOOS=windows \
go build -tags box -ldflags "-s -w" -o libhaicam_ext.a -buildmode=c-archive *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/win64/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/win64/
cp -f libhaicam_ext.h box_proj/build/win64/
cp -f libhaicam_ext.a box_proj/build/win64/
rm libhaicam_ext.h
rm libhaicam_ext.a
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "win64 build done"

echo "prepare libhaicam-p2p-onvif.a win32"
cd box_proj/build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake -DANDROID_ABI=win32 \
-DCMAKE_TOOLCHAIN_FILE=../../../build/toolchain/win32.toolchain.cmake ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ../../
echo "box_proj/build done"

CGO_ENABLED=1 \
CC=i686-w64-mingw32-gcc \
GOOS=windows \
GOARCH=386 \
CGO_LDFLAGS=-Wl,-soname,libhaicam_ext.dll \
go build -tags box -ldflags "-s -w" -o libhaicam_ext.dll -buildmode=c-shared *.go
cp -f box_proj/build/libhaicam-p2p-onvif.a box_proj/build/win32/
cp -f box_proj/build/libhaicam-ifaddrs.a box_proj/build/win32/
cp -f libhaicam_ext.h box_proj/build/win32/
cp -f libhaicam_ext.dll box_proj/build/win32/
rm libhaicam_ext.h
rm libhaicam_ext.dll
rm box_proj/build/libhaicam-p2p-onvif.a
rm box_proj/build/libhaicam-ifaddrs.a
echo "win32 build done"
