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