cmake_minimum_required(VERSION 3.6)

project(haicam-p2p C CXX)

if (ANDROID_ABI MATCHES "linux32")
    set(CMAKE_C_FLAGS "-m32")
    set(CMAKE_CXX_FLAGS "-m32")
endif()

if (ANDROID_ABI MATCHES "gm8136")
    set(CMAKE_CXX_FLAGS "-std=c++0x -Dnullptr=NULL")
endif()

if (ANDROID_ABI MATCHES "rpi32")
    set(CMAKE_C_FLAGS "-fPIC")
    set(CMAKE_CXX_FLAGS "-fPIC")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/build/${ANDROID_ABI}
)       

if (ANDROID_ABI MATCHES "win32")

file(GLOB SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../P2Peer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../discovery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../websocket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../dlfcn.c
)

else()

file(GLOB SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../P2Peer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../discovery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../websocket.cpp
)

endif()

add_library(
    haicam-p2p-onvif
    ${SRCS}
)

file(GLOB SRCS_2
    ${CMAKE_CURRENT_SOURCE_DIR}/../ifaddrs.cpp
)

add_library(
    haicam-ifaddrs
    ${SRCS_2}
)

