# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/haicam/workspace/golang/box_proj

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/haicam/workspace/golang/box_proj/build

# Include any dependencies generated for this target.
include CMakeFiles/haicam-ifaddrs.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/haicam-ifaddrs.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/haicam-ifaddrs.dir/flags.make

CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.o: CMakeFiles/haicam-ifaddrs.dir/flags.make
CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.o: /home/haicam/workspace/golang/ifaddrs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/haicam/workspace/golang/box_proj/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.o"
	/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ --target=aarch64-none-linux-android21 --sysroot=/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/sysroot  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.o -c /home/haicam/workspace/golang/ifaddrs.cpp

CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.i"
	/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ --target=aarch64-none-linux-android21 --sysroot=/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/sysroot $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/haicam/workspace/golang/ifaddrs.cpp > CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.i

CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.s"
	/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ --target=aarch64-none-linux-android21 --sysroot=/home/haicam/toolchain/android-ndk-r24/toolchains/llvm/prebuilt/linux-x86_64/sysroot $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/haicam/workspace/golang/ifaddrs.cpp -o CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.s

# Object files for target haicam-ifaddrs
haicam__ifaddrs_OBJECTS = \
"CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.o"

# External object files for target haicam-ifaddrs
haicam__ifaddrs_EXTERNAL_OBJECTS =

libhaicam-ifaddrs.a: CMakeFiles/haicam-ifaddrs.dir/home/haicam/workspace/golang/ifaddrs.cpp.o
libhaicam-ifaddrs.a: CMakeFiles/haicam-ifaddrs.dir/build.make
libhaicam-ifaddrs.a: CMakeFiles/haicam-ifaddrs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/haicam/workspace/golang/box_proj/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libhaicam-ifaddrs.a"
	$(CMAKE_COMMAND) -P CMakeFiles/haicam-ifaddrs.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/haicam-ifaddrs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/haicam-ifaddrs.dir/build: libhaicam-ifaddrs.a

.PHONY : CMakeFiles/haicam-ifaddrs.dir/build

CMakeFiles/haicam-ifaddrs.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/haicam-ifaddrs.dir/cmake_clean.cmake
.PHONY : CMakeFiles/haicam-ifaddrs.dir/clean

CMakeFiles/haicam-ifaddrs.dir/depend:
	cd /home/haicam/workspace/golang/box_proj/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/haicam/workspace/golang/box_proj /home/haicam/workspace/golang/box_proj /home/haicam/workspace/golang/box_proj/build /home/haicam/workspace/golang/box_proj/build /home/haicam/workspace/golang/box_proj/build/CMakeFiles/haicam-ifaddrs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/haicam-ifaddrs.dir/depend
