set(CMAKE_SYSTEM_NAME "Linux")
set(TOOLCHAIN_PREFIX /usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/bin/arm-unknown-linux-uclibcgnueabi)

# specify the cross compiler
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH
    /usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/sysroot
)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_AR ${TOOLCHAIN_PREFIX}-ar CACHE FILEPATH "ar")
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}-ranlib CACHE FILEPATH "ranlib")