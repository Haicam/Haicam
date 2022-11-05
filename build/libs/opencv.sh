test -d /tmp/opencv && rm -rf /tmp/opencv
cd /tmp
git clone https://github.com/opencv/opencv.git
cd opencv
# mini gcc 4.8
git checkout 4.6.0

mkdir build
cd build

#linux x86_64
#https://github.com/Qengineering/Install-OpenCV-Raspberry-Pi-64-bits/blob/main/OpenCV-4-6-0.sh
#PKG_CONFIG_PATH=$PKG_CONFIG_PATH:my/path/to/pkgconfig cmake <some args>
#PKG_CONFIG_PATH=/home/haicam/workspace/libs/ffmpeg/linux/x86_64/lib/pkgconfig 
#OpenCV disables pkg-config to avoid using of host libraries. Consider using PKG_CONFIG_LIBDIR to specify target SYSROOT
# apt-get install libgtk-3-dev
# pkg-config --cflags --libs gtk+-3.0
PKG_CONFIG_LIBDIR=/home/haicam/workspace/libs/ffmpeg/linux/x86_64/lib/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig:/usr/share/pkgconfig  cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake \
-DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/opencv/linux/x86_64 \
-D CMAKE_BUILD_TYPE=RELEASE \
-D BUILD_TESTS=OFF \
-D BUILD_JAVA=OFF \
-D BUILD_OBJC=OFF \
-D BUILD_KOTLIN_EXTENSIONS=OFF \
-D BUILD_opencv_apps=OFF \
-D BUILD_opencv_python2=OFF \
-D BUILD_SHARED_LIBS=OFF \
-D WITH_FFMPEG=ON \
-D WITH_V4L=ON \
-D WITH_LIBV4L=ON \
-D WITH_VTK=OFF \
-D WITH_GTK=ON \
-D WITH_QT=OFF \
-D INSTALL_C_EXAMPLES=OFF \
-D INSTALL_PYTHON_EXAMPLES=OFF \
-D BUILD_EXAMPLES=OFF ..

make -j`nproc` && make install
rm -rf *

#windows x86_64
PKG_CONFIG_LIBDIR=/home/haicam/workspace/libs/ffmpeg/windows/x86_64/lib/pkgconfig cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/win64.toolchain.cmake \
-DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/opencv/windows/x86_64 \
-D CMAKE_BUILD_TYPE=RELEASE \
-D BUILD_TESTS=OFF \
-D BUILD_JAVA=OFF \
-D BUILD_OBJC=OFF \
-D BUILD_KOTLIN_EXTENSIONS=OFF \
-D BUILD_opencv_apps=OFF \
-D BUILD_opencv_python2=OFF \
-D BUILD_SHARED_LIBS=OFF \
-D WITH_FFMPEG=ON \
-D WITH_VTK=OFF \
-D WITH_GTK=ON \
-D WITH_QT=OFF \
-D INSTALL_C_EXAMPLES=OFF \
-D INSTALL_PYTHON_EXAMPLES=OFF \
-D BUILD_EXAMPLES=OFF ..

make -j`nproc` && make install
rm -rf *

#mac x86_64
PKG_CONFIG_LIBDIR=/home/haicam/workspace/libs/ffmpeg/mac/x86_64/lib/pkgconfig cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/mac-x86_64.toolchain.cmake \
-DCMAKE_INSTALL_PREFIX=/home/haicam/workspace/libs/opencv/mac/x86_64 \
-D CMAKE_BUILD_TYPE=RELEASE \
-D BUILD_TESTS=OFF \
-D BUILD_JAVA=OFF \
-D BUILD_opencv_objdetect=OFF \
-D BUILD_opencv_dnn=OFF \
-D BUILD_KOTLIN_EXTENSIONS=OFF \
-D BUILD_opencv_apps=OFF \
-D BUILD_opencv_python2=OFF \
-D BUILD_SHARED_LIBS=OFF \
-D WITH_FFMPEG=ON \
-D WITH_V4L=ON \
-D WITH_LIBV4L=ON \
-D WITH_VTK=OFF \
-D WITH_GTK=ON \
-D WITH_QT=OFF \
-D INSTALL_C_EXAMPLES=OFF \
-D INSTALL_PYTHON_EXAMPLES=OFF \
-D BUILD_EXAMPLES=OFF ..

make -j`nproc` && make install
rm -rf *