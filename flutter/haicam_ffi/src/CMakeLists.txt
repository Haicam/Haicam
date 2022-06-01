# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

project(haicam_ffi_library VERSION 0.0.1 LANGUAGES C)

add_library(haicam_ffi SHARED
  "haicam_ffi.c"
)

set_target_properties(haicam_ffi PROPERTIES
  PUBLIC_HEADER haicam_ffi.h
  OUTPUT_NAME "haicam_ffi"
)

target_compile_definitions(haicam_ffi PUBLIC DART_SHARED_LIB)
