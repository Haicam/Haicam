echo "prepare libhaicam-p2p.a ios arm64"

cd build
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake -DANDROID_ABI=ios-arm64 -DCMAKE_TOOLCHAIN_FILE=../../build/toolchain/ios-arm64.toolchain.cmake ..
make
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cd ..

CGO_ENABLED=1 \
GOOS=darwin \
GOARCH=arm64 \
CC=/home/haicam/toolchain/iPhoneOS15.4/bin/arm-apple-darwin11-clang \
go build -tags app -ldflags "-s -w" -buildmode=c-archive -o libhaicam_ext.a p2p.go

cp -f build/libhaicam-p2p.a build/ios-arm64/
cp -f libhaicam_ext.h build/ios-arm64/
cp -f libhaicam_ext.a build/ios-arm64/
cp -f build/libhaicam-ifaddrs.a build/ios-arm64/
rm libhaicam_ext.h
rm libhaicam_ext.a
rm build/libhaicam-p2p.a
rm build/libhaicam-ifaddrs.a
echo "ios arm64 build done"
