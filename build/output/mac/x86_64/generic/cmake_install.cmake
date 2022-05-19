# Install script for directory: /home/haicam/workspace

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/haicam/workspace/bin/mac/x86_64/generic/haicam-app")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/haicam/workspace/bin/mac/x86_64/generic" TYPE EXECUTABLE FILES "/home/haicam/workspace/build/output/mac/x86_64/generic/haicam-app")
  if(EXISTS "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/haicam-app" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/haicam-app")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-strip" -u -r "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/haicam-app")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/haicam/workspace/bin/mac/x86_64/generic/haicam-test")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/haicam/workspace/bin/mac/x86_64/generic" TYPE EXECUTABLE FILES "/home/haicam/workspace/build/output/mac/x86_64/generic/haicam-test")
  if(EXISTS "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/haicam-test" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/haicam-test")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-strip" -u -r "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/haicam-test")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core.dylib")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/haicam/workspace/bin/mac/x86_64/generic/libs" TYPE SHARED_LIBRARY FILES "/home/haicam/workspace/build/output/mac/x86_64/generic/libhaicam-core.dylib")
  if(EXISTS "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core.dylib")
    execute_process(COMMAND "/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-install_name_tool"
      -id "libhaicam-core.dylib"
      "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-strip" -x "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core_a.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/haicam/workspace/bin/mac/x86_64/generic/libs" TYPE STATIC_LIBRARY FILES "/home/haicam/workspace/build/output/mac/x86_64/generic/libhaicam-core_a.a")
  if(EXISTS "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core_a.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core_a.a")
    execute_process(COMMAND "/home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4-ranlib" "$ENV{DESTDIR}/home/haicam/workspace/bin/mac/x86_64/generic/libs/libhaicam-core_a.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/haicam/workspace/build/output/mac/x86_64/generic/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
