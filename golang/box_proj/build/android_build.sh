#NDK=/Users/frankhe/Apps/adt-bundle/sdk/ndk/21.3.6528147
NDK=/home/frankhe/Android/Sdk/ndk/24.0.8215888

make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake \
-DCMAKE_TOOLCHAIN_FILE=${NDK}/build/cmake/android.toolchain.cmake \
-DANDROID_ABI=armeabi-v7a -DANDROID_ARM_NEON=ON  \
-DANDROID_NATIVE_API_LEVEL=21 ..
make
cp -f libhaicam-p2p-onvif.a ./armeabi-v7a/
rm libhaicam-p2p-onvif.a
echo "armeabi-v7a build done"

make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
cmake \
-DCMAKE_TOOLCHAIN_FILE=${NDK}/build/cmake/android.toolchain.cmake \
-DANDROID_ABI=arm64-v8a \
-DANDROID_NATIVE_API_LEVEL=21 ..
make
cp -f libhaicam-p2p-onvif.a ./arm64-v8a/
rm libhaicam-p2p-onvif.a
echo "arm64-v8a build done"
make clean
rm CMakeCache.txt; rm Makefile; rm cmake_install.cmake; rm -rf CMakeFiles
