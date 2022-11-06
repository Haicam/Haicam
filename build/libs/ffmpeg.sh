test -d /tmp/ffmpeg && rm -rf /tmp/ffmpeg
cd /tmp
git clone https://github.com/FFmpeg/FFmpeg.git
cd FFmpeg
git checkout n4.4.3

mkdir build
cd build

#linux x86_64
#libopenh264, h264_v4l2m2m, opus, aac
#https://github.com/cmeng-git/ffmpeg-android/blob/master/_ffmpeg_build.sh
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/haicam/workspace/libs/openh264/linux/x86_64/lib/pkgconfig ../configure --prefix=/home/haicam/workspace/libs/ffmpeg/linux/x86_64 \
--enable-pic \
--enable-avresample \
--enable-libopenh264 \
--disable-doc \
--disable-debug \
--enable-static \
--disable-shared \
--disable-programs \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-iconv \
--extra-cflags="-fPIE -pie" \
--extra-ldflags="-fPIE -pie"

make -j`nproc` && make install
rm -rf *

#windows x86_64
#libopenh264, h264_v4l2m2m, opus, aac
#https://github.com/cmeng-git/ffmpeg-android/blob/master/_ffmpeg_build.sh
#apt-get install -y pkg-config-mingw-w64-x86-64 dpkg-dev 
# update ../configure line  6412 to below, disable faield pkg_config check
#-enabled libopenh264       && require_pkg_config libopenh264 openh264 wels/codec_api.h WelsGetCodecVersion
#+enabled libopenh264
../configure --prefix=/home/haicam/workspace/libs/ffmpeg/windows/x86_64 \
--cross-prefix=x86_64-w64-mingw32- \
--arch=x86_64 \
--target-os=mingw32 \
--enable-pic \
--enable-avresample \
--enable-libopenh264 \
--disable-doc \
--disable-debug \
--enable-static \
--disable-shared \
--disable-programs \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-iconv \
--extra-cflags="-fPIE -pie -I/home/haicam/workspace/libs/openh264/windows/x86_64/include" \
--extra-ldflags="-fPIE -pie -L/home/haicam/workspace/libs/openh264/windows/x86_64/lib"

make -j`nproc` && make install
rm -rf *

#mac x86_64
#libopenh264, h264_v4l2m2m, opus, aac
#https://github.com/cmeng-git/ffmpeg-android/blob/master/_ffmpeg_build.sh
# update ../configure line  6412 to below, disable faield pkg_config check
#-enabled libopenh264       && require_pkg_config libopenh264 openh264 wels/codec_api.h WelsGetCodecVersion
#+enabled libopenh264
(
export TOOLCHAIN_PREFIX=/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4
export CC=${TOOLCHAIN_PREFIX}-clang
export CXX=${TOOLCHAIN_PREFIX}-clang++
export AR=${TOOLCHAIN_PREFIX}-ar
export RANLIB=${TOOLCHAIN_PREFIX}-ranlib
export LIBTOOL=${TOOLCHAIN_PREFIX}-libtool

../configure --prefix=/home/haicam/workspace/libs/ffmpeg/mac/x86_64 \
--cc=${CC} \
--cxx=${CXX} \
--enable-cross-compile \
--enable-pic \
--enable-avresample \
--enable-libopenh264 \
--disable-doc \
--disable-debug \
--enable-static \
--disable-shared \
--disable-programs \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-iconv \
--extra-cflags="-fPIE -pie -I/home/haicam/workspace/libs/openh264/mac/x86_64/include" \
--extra-ldflags="-fPIE -pie -L/home/haicam/workspace/libs/openh264/mac/x86_64/lib"

make -j`nproc` && make install
rm -rf *
)

#rpi armv7
# update ../configure line  6412 to below, disable faield pkg_config check
#-enabled libopenh264       && require_pkg_config libopenh264 openh264 wels/codec_api.h WelsGetCodecVersion
#+enabled libopenh264
../configure --prefix=/home/haicam/workspace/libs/ffmpeg/rpi/armv7 \
--cross-prefix=/home/haicam/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi- \
--target-os=linux \
--arch=arm \
--enable-pic \
--enable-avresample \
--enable-libopenh264 \
--disable-doc \
--disable-debug \
--enable-static \
--disable-shared \
--disable-programs \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-iconv \
--extra-cflags="-fPIE -pie -I/home/haicam/workspace/libs/openh264/rpi/armv7/include" \
--extra-ldflags="-fPIE -pie -L/home/haicam/workspace/libs/openh264/rpi/armv7/lib"

make -j && make install
rm -rf *
