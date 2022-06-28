set(CMAKE_SYSTEM_NAME "Linux")
set(TOOLCHAIN_PREFIX /home/haicam/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi)

# specify the cross compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH
    /home/haicam/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabi/arm-linux-gnueabi/libc
)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_AR ${TOOLCHAIN_PREFIX}-ar CACHE FILEPATH "ar")
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}-ranlib CACHE FILEPATH "ranlib")