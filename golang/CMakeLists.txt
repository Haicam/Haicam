cmake_minimum_required(VERSION 3.6)

project(haicam-p2p C CXX)

set(HAICAM_CORE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../CloudCameraHub-core)

include_directories(
    ${HAICAM_CORE_PATH}/Hub_Embedded/
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/build/${ANDROID_ABI}
)

file(GLOB SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/P2Peer.cpp
)

file(GLOB SRCS_2
    ${CMAKE_CURRENT_SOURCE_DIR}/ifaddrs.cpp
)

add_library(
    haicam-p2p
    ${SRCS}
)

add_library(
    haicam-ifaddrs
    ${SRCS_2}
)