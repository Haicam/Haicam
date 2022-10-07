# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

project(haicam_ffi_library VERSION 0.0.1 LANGUAGES C CXX)

#CMake PROJECT_SOURCE_DIR: .../haicam-mobile/haicam/linux/flutter/ephemeral/.plugin_symlinks/haicam_ffi/src
message(STATUS "================ PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")
message(STATUS "================ CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "================ CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "================ ANDROID_ABI: ${ANDROID_ABI}")
message(STATUS "================ CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")

#default
set(HAICAM_PROJECT_PATH ${PROJECT_SOURCE_DIR}/../../../../../../../../Haicam)
set(HAICAM_PLATFORM "linux")
set(HAICAM_TARGET "x86_64")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(HAICAM_PLATFORM "linux")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(HAICAM_TARGET "x86_64")
  endif()
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")
  set(HAICAM_PROJECT_PATH ${PROJECT_SOURCE_DIR}/../../../../Haicam)
  set(HAICAM_PLATFORM "android")
  if(ANDROID_ABI MATCHES "arm64-v8a")
    set(HAICAM_TARGET "arm64-v8a")
  elseif (ANDROID_ABI MATCHES "armeabi-v7a") 
    set(HAICAM_TARGET "armeabi-v7a")
  else()
    message(FATAL_ERROR "ANDROID_ABI: [${ANDROID_ABI}] does not support, CMake will exit!")
  endif()
endif()

message(WARNING "================ HAICAM_PLATFORM: ${HAICAM_PLATFORM}")
message(WARNING "================ HAICAM_TARGET: ${HAICAM_TARGET}")
message(WARNING "================ HAICAM_PROJECT_PATH: ${HAICAM_PROJECT_PATH}")

include_directories(
  ${HAICAM_PROJECT_PATH}/libs/openssl/${HAICAM_PLATFORM}/${HAICAM_TARGET}/include
  ${HAICAM_PROJECT_PATH}/libs/jsoncpp
)

link_directories(
  ${HAICAM_PROJECT_PATH}/libs/openssl/${HAICAM_PLATFORM}/${HAICAM_TARGET}/lib
)

add_library(haicam_ffi SHARED
  "haicam_ffi.c"
  ${HAICAM_PROJECT_PATH}/libs/jsoncpp/jsoncpp.cpp
)

target_link_libraries (haicam_ffi
  ssl.a
  crypto.a
)

set_target_properties(haicam_ffi PROPERTIES
  PUBLIC_HEADER haicam_ffi.h
  OUTPUT_NAME "haicam_ffi"
)

target_compile_definitions(haicam_ffi PUBLIC DART_SHARED_LIB)
