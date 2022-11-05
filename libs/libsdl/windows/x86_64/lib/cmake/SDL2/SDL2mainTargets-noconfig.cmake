#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SDL2::SDL2main" for configuration ""
set_property(TARGET SDL2::SDL2main APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(SDL2::SDL2main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libSDL2main.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS SDL2::SDL2main )
list(APPEND _IMPORT_CHECK_FILES_FOR_SDL2::SDL2main "${_IMPORT_PREFIX}/lib/libSDL2main.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
