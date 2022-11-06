test -d /tmp/curl && rm -rf /tmp/curl
cd /tmp
git clone https://github.com/curl/curl.git
cd curl
git checkout curl-7_86_0

mkdir build
cd build

#linux x86_64
cmake -DCMAKE_C_FLAGS='-fPIE' \
 -DOPENSSL_INCLUDE_DIR=/home/haicam/workspace/libs/openssl/linux/x86_64/include \
 -DOPENSSL_CRYPTO_LIBRARY=/home/haicam/workspace/libs/openssl/linux/x86_64/lib \
 -DOPENSSL_SSL_LIBRARY=/home/haicam/workspace/libs/openssl/linux/x86_64/lib \
 -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake \
 -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/curl/linux/x86_64 \
 -DBUILD_SHARED_LIBS=OFF \
 -DBUILD_CURL_EXE=OFF \
 -DBUILD_TESTING=OFF ..
make -j && make install
rm -rf *

#windows x86_64
cmake -DCMAKE_C_FLAGS='-fPIE' \
 -DOPENSSL_INCLUDE_DIR=/home/haicam/workspace/libs/openssl/windows/x86_64/include \
 -DOPENSSL_CRYPTO_LIBRARY=/home/haicam/workspace/libs/openssl/windows/x86_64/lib \
 -DOPENSSL_SSL_LIBRARY=/home/haicam/workspace/libs/openssl/windows/x86_64/lib \
 -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win64.toolchain.cmake \
 -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/curl/windows/x86_64 \
 -DBUILD_SHARED_LIBS=OFF \
 -DBUILD_CURL_EXE=OFF \
 -DBUILD_TESTING=OFF ..
make -j && make install
rm -rf *

#mac x86_64
cmake -DCMAKE_C_FLAGS='-fPIE' \
 -DOPENSSL_INCLUDE_DIR=/home/haicam/workspace/libs/openssl/mac/x86_64/include \
 -DOPENSSL_CRYPTO_LIBRARY=/home/haicam/workspace/libs/openssl/mac/x86_64/lib \
 -DOPENSSL_SSL_LIBRARY=/home/haicam/workspace/libs/openssl/mac/x86_64/lib \
 -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/mac-x86_64.toolchain.cmake \
 -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/curl/mac/x86_64 \
 -DBUILD_SHARED_LIBS=OFF \
 -DBUILD_CURL_EXE=OFF \
 -DBUILD_TESTING=OFF ..
make -j && make install
rm -rf *

#gm8136 armv5
cmake -DCMAKE_C_FLAGS='-fPIE' \
 -DOPENSSL_INCLUDE_DIR=/home/haicam/workspace/libs/openssl/gm8136/armv5/include \
 -DOPENSSL_CRYPTO_LIBRARY=/home/haicam/workspace/libs/openssl/gm8136/armv5/lib \
 -DOPENSSL_SSL_LIBRARY=/home/haicam/workspace/libs/openssl/gm8136/armv5/lib \
 -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-gm8136.toolchain.cmake \
 -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/curl/gm8136/armv5 \
 -DBUILD_SHARED_LIBS=OFF \
 -DBUILD_CURL_EXE=OFF \
 -DBUILD_TESTING=OFF ..
make -j`nproc` && make install
rm -rf *

#rpi armv7
cmake -DCMAKE_C_FLAGS='-fPIE' \
 -DOPENSSL_INCLUDE_DIR=/home/haicam/workspace/libs/openssl/rpi/armv7/include \
 -DOPENSSL_CRYPTO_LIBRARY=/home/haicam/workspace/libs/openssl/rpi/armv7/lib \
 -DOPENSSL_SSL_LIBRARY=/home/haicam/workspace/libs/openssl/rpi/armv7/lib \
 -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-rpi.toolchain.cmake \
 -DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/curl/rpi/armv7 \
 -DBUILD_SHARED_LIBS=OFF \
 -DBUILD_CURL_EXE=OFF \
 -DBUILD_TESTING=OFF ..
make -j && make install
rm -rf *

