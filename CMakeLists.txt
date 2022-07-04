cmake_minimum_required(VERSION 3.4.1)

project(haicam-core)

set(CMAKE_BUILD_TYPE "Debug")

if (HAICAM_PLATFORM MATCHES "gm8136")
    add_definitions(-DHAICAM_GM8135_GCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O0 -ggdb3 ")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -ggdb3 ")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libuv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/zbar/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/quid/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/curl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ffmpeg/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/openssl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/jsoncpp
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libuv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/zbar/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/quid/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/curl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ffmpeg/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/openssl/${HAICAM_PLATFORM}/${HAICAM_ARCH}/lib
)

file(GLOB HAICAM_CORE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${HAICAM_PLATFORM}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${HAICAM_PLATFORM}/${HAICAM_TARGET}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/jsoncpp/jsoncpp.cpp
)

file(GLOB HAICAM_APP_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/example/main.cpp
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

add_executable(haicam-app 
    ${HAICAM_APP_SRCS} 
)

if (HAICAM_PLATFORM MATCHES "android")

    target_link_libraries (haicam-core
        uv_a.a
    )

    target_link_libraries (haicam-app
        haicam-core_a
        uv_a.a
    )

elseif (HAICAM_PLATFORM MATCHES "windows")

    target_link_libraries (haicam-core
        uv_a.a
        wsock32.a ws2_32.a wldap32.a iphlpapi.a secur32.a userenv.a
    )

    target_link_libraries (haicam-app
        haicam-core_a
        uv_a.a
        wsock32.a ws2_32.a wldap32.a iphlpapi.a secur32.a userenv.a
    )

elseif (HAICAM_PLATFORM MATCHES "mac" OR HAICAM_PLATFORM MATCHES "ios")

    target_link_libraries (haicam-core
        uv_a.a
    )

    target_link_libraries (haicam-app
        haicam-core_a
        uv_a.a
        pthread
        dl
    )

else()

    target_link_libraries (haicam-app
        haicam-core_a
        uv_a.a
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
    )

elseif (HAICAM_PLATFORM MATCHES "windows")

    target_link_libraries (haicam-test
        haicam-core_a
        uv_a.a
        gtest.a
        gtest_main.a
        gmock.a
        gmock_main.a
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
        pthread
        dl
    )

else()

    target_link_libraries (haicam-test
        haicam-core_a
        uv_a.a
        gtest.a
        gtest_main.a
        gmock.a
        gmock_main.a
        pthread
        dl
    )

endif()

install(TARGETS haicam-app haicam-test haicam-core haicam-core_a
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_ARCH}/${HAICAM_TARGET}
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_ARCH}/${HAICAM_TARGET}/libs
    ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_ARCH}/${HAICAM_TARGET}/libs
)

