set(CMAKE_SYSTEM_NAME "Darwin")
set(TOOLCHAIN_PREFIX /home/haicam/toolchain/MacOSX12.3/bin/x86_64-apple-darwin21.4)

# specify the cross compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-clang)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-clang++)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH
    /home/haicam/toolchain/MacOSX12.3/SDK/MacOSX12.3.sdk
)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_AR ${TOOLCHAIN_PREFIX}-ar CACHE FILEPATH "ar")
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}-ranlib CACHE FILEPATH "ranlib")
set(CMAKE_INSTALL_NAME_TOOL ${TOOLCHAIN_PREFIX}-install_name_tool CACHE FILEPATH "install_name_tool")