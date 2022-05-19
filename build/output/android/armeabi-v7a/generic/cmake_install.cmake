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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/haicam/workspace/bin/android/armeabi-v7a/generic/haicam-app")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/haicam/workspace/bin/android/armeabi-v7a/generic" TYPE EXECUTABLE FILES "/home/haicam/workspace/build/output/android/armeabi-v7a/generic/haicam-app")
  if(EXISTS "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/haicam-app" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/haicam-app")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip" "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/haicam-app")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/haicam/workspace/bin/android/armeabi-v7a/generic/haicam-test")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/haicam/workspace/bin/android/armeabi-v7a/generic" TYPE EXECUTABLE FILES "/home/haicam/workspace/build/output/android/armeabi-v7a/generic/haicam-test")
  if(EXISTS "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/haicam-test" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/haicam-test")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip" "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/haicam-test")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/haicam/workspace/bin/android/armeabi-v7a/generic/libs/libhaicam-core.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/haicam/workspace/bin/android/armeabi-v7a/generic/libs" TYPE SHARED_LIBRARY FILES "/home/haicam/workspace/build/output/android/armeabi-v7a/generic/libhaicam-core.so")
  if(EXISTS "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/libs/libhaicam-core.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/libs/libhaicam-core.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip" "$ENV{DESTDIR}/home/haicam/workspace/bin/android/armeabi-v7a/generic/libs/libhaicam-core.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/haicam/workspace/bin/android/armeabi-v7a/generic/libs/libhaicam-core_a.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/haicam/workspace/bin/android/armeabi-v7a/generic/libs" TYPE STATIC_LIBRARY FILES "/home/haicam/workspace/build/output/android/armeabi-v7a/generic/libhaicam-core_a.a")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/haicam/workspace/build/output/android/armeabi-v7a/generic/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
