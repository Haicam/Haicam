#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "uv" for configuration ""
set_property(TARGET uv APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(uv PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libuv.1.0.0.dylib"
  IMPORTED_SONAME_NOCONFIG "libuv.1.dylib"
  )

list(APPEND _IMPORT_CHECK_TARGETS uv )
list(APPEND _IMPORT_CHECK_FILES_FOR_uv "${_IMPORT_PREFIX}/lib/libuv.1.0.0.dylib" )

# Import target "uv_a" for configuration ""
set_property(TARGET uv_a APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(uv_a PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libuv_a.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS uv_a )
list(APPEND _IMPORT_CHECK_FILES_FOR_uv_a "${_IMPORT_PREFIX}/lib/libuv_a.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
