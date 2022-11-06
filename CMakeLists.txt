cmake_minimum_required(VERSION 3.4.1)

project(haicam-core VERSION 0.0.2 LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE "Debug")

if (HAICAM_PLATFORM MATCHES "windows")
    add_definitions(-DHAICAM_PLATFORM_WINDOWS)
elseif (HAICAM_PLATFORM MATCHES "gm8136")
    add_definitions(-DHAICAM_PLATFORM_GM8136)
else()
    add_definitions(-DHAICAM_PLATFORM_LINUX)
endif()

if (HAICAM_PLATFORM MATCHES "gm8136")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -pie -Wno-psabi -O0 -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fPIE -pie -frtti -fexceptions -Wno-psabi -O0 -ggdb3 -Wno-pointer-arith")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -pie -O0 -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIE -pie -frtti -fexceptions -O0 -ggdb3 -Wno-pointer-arith")

    find_package(PkgConfig)
    pkg_search_module(GTK REQUIRED IMPORTED_TARGET gtk+-3.0)

endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cameras/${HAICAM_PLATFORM}/${CAMERA_SDK}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libuv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/zbar/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/quid/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/curl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ffmpeg/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/openssl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libsdl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/chan/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/jsoncpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite3
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${HAICAM_PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${HAICAM_PLATFORM}/${HAICAM_TARGET}
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cameras/${HAICAM_PLATFORM}/${CAMERA_SDK}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libuv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/zbar/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/quid/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/curl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ffmpeg/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/openh264/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib/3rdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/openssl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libsdl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/chan/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
)

file(GLOB HAICAM_CORE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${HAICAM_PLATFORM}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${HAICAM_PLATFORM}/${HAICAM_TARGET}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${HAICAM_PLATFORM}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${HAICAM_PLATFORM}/${HAICAM_TARGET}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/jsoncpp/jsoncpp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite3/sqlite3.c
)

file(GLOB HAICAM_APP_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera/main.cpp
)

file(GLOB HAICAM_TEST_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/${HAICAM_PLATFORM}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/${HAICAM_PLATFORM}/${HAICAM_TARGET}/*.cpp
)

add_library(haicam-core_a STATIC 
    ${HAICAM_CORE_SRCS}
)

add_library(haicam-core SHARED 
    ${HAICAM_CORE_SRCS}
)

add_executable(haicam-camera-app 
    ${HAICAM_APP_SRCS} 
)

if (HAICAM_PLATFORM MATCHES "android")

    target_link_libraries (haicam-core
        uv_a.a
        ssl.a
        crypto.a
    )

    target_link_libraries (haicam-camera-app
        haicam-core_a
        uv_a.a
        ssl.a
        crypto.a
    )

elseif (HAICAM_PLATFORM MATCHES "windows")

    target_link_libraries (haicam-core
        uv_a.a
        ssl.a
        crypto.a
        wsock32.a ws2_32.a wldap32.a iphlpapi.a secur32.a userenv.a
    )

    target_link_libraries (haicam-camera-app
        haicam-core_a
        uv_a.a
        ssl.a
        crypto.a
        wsock32.a ws2_32.a wldap32.a iphlpapi.a secur32.a userenv.a
    )

elseif (HAICAM_PLATFORM MATCHES "mac" OR HAICAM_PLATFORM MATCHES "ios")

    target_link_libraries (haicam-core
        uv_a.a
        ssl.a
        crypto.a
    )

    target_link_libraries (haicam-camera-app
        haicam-core_a
        uv_a.a
        ssl.a
        crypto.a
        pthread
        dl
    )

elseif (HAICAM_PLATFORM MATCHES "linux")
    target_link_libraries (haicam-camera-app
        haicam-core_a
        uv_a.a
        ssl.a
        crypto.a
        SDL2.a
        pthread
        dl
    )

else()

    target_link_libraries (haicam-camera-app
        haicam-core_a
        uv_a.a
        ssl.a
        crypto.a
        pthread
        dl
    )

endif()

add_executable(haicam-test 
    ${HAICAM_TEST_SRCS} 
)

if (HAICAM_PLATFORM MATCHES "android")

    target_link_libraries (haicam-test
        haicam-core_a
        uv_a.a
        gtest.a
        gtest_main.a
        gmock.a
        gmock_main.a
        ssl.a
        crypto.a
    )

elseif (HAICAM_PLATFORM MATCHES "windows")

    target_link_libraries (haicam-test
        haicam-core_a
        uv_a.a
        gtest.a
        gtest_main.a
        gmock.a
        gmock_main.a
        ssl.a
        crypto.a
        wsock32.a ws2_32.a wldap32.a iphlpapi.a secur32.a userenv.a
    )

elseif (HAICAM_PLATFORM MATCHES "mac" OR HAICAM_PLATFORM MATCHES "ios")

    target_link_libraries (haicam-test
        haicam-core_a
        uv_a.a
        gtest.a
        gtest_main.a
        gmock.a
        gmock_main.a
        ssl.a
        crypto.a
        pthread
        dl
    )

elseif (HAICAM_PLATFORM MATCHES "linux")
    target_link_libraries (haicam-test
        haicam-core_a
        uv_a.a
        chan.a
        gtest.a
        gtest_main.a
        gmock.a
        gmock_main.a
        ssl.a
        crypto.a
        SDL2.a
        opencv_videoio.a
        opencv_imgcodecs.a
        opencv_imgproc.a
        opencv_features2d.a
        opencv_highgui.a
        opencv_core.a
        avfilter.a
        avdevice.a
        avformat.a
        avcodec.a
        swresample.a
        swscale.a
        avutil.a
        openh264.a
        jpeg-turbo.a
        png.a
        webp.a
        tiff.a
        openjp2.a 
        PkgConfig::GTK
        z
        lzma
        pthread
        dl
    )

else()

    target_link_libraries (haicam-test
        haicam-core_a
        uv_a.a
        chan.a
        gtest.a
        gtest_main.a
        gmock.a
        gmock_main.a
        ssl.a
        crypto.a
        gm
        pthread
        dl
    )

endif()

install(TARGETS haicam-camera-app haicam-test haicam-core haicam-core_a
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_ARCH}/${HAICAM_TARGET}
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_ARCH}/${HAICAM_TARGET}/libs
    ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_ARCH}/${HAICAM_TARGET}/libs
)

