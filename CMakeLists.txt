cmake_minimum_required(VERSION 3.4.1)

project(haicam-core)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/unity/${HAICAM_PLATFORM}/${HAICAM_TARGET}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libuv/${HAICAM_PLATFORM}/${HAICAM_TARGET}/include
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/unity/${HAICAM_PLATFORM}/${HAICAM_TARGET}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/libuv/${HAICAM_PLATFORM}/${HAICAM_TARGET}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/build/output/${HAICAM_PLATFORM}/${HAICAM_TARGET}
)

file(GLOB HAICAM_CORE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HaicamContext.cpp
)

file(GLOB HAICAM_APP_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_library(haicam-core_a STATIC 
    ${HAICAM_CORE_SRCS}
)

add_library(haicam-core SHARED 
    ${HAICAM_CORE_SRCS}
)

add_executable(haicam-app 
    ${HAICAM_APP_SRCS} 
)

if (HAICAM_PLATFORM MATCHES "android")

    target_link_libraries (haicam-core
        uv_a.a
        unity.a
    )

    target_link_libraries (haicam-app
        haicam-core_a
        uv_a.a
        unity.a
    )

else()

    target_link_libraries (haicam-app
        haicam-core_a
        uv_a.a
        unity.a
        pthread
        dl
    )

endif()

install(TARGETS haicam-app haicam-core haicam-core_a
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_TARGET}
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_TARGET}/libs
    ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/${HAICAM_PLATFORM}/${HAICAM_TARGET}/libs
)

