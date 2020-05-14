cmake_minimum_required(VERSION 3.15)
project(libav_test)
set(CMAKE_CXX_STANDARD 20)

set(CLANG_COMPILE_FLAGS "-fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_COMPILE_FLAGS}")

add_subdirectory(lib/FFmpeg)

list(APPEND SOURCES
        libav_wrappers/AVPacket.cpp
        )

#add_compile_options("$<$<CONFIG:DEBUG>:-DTEST>")

add_executable(libav_test main.cpp ${SOURCES} libav_wrappers/AVPacket.h)

target_link_libraries(libav_test FFmpeg)
