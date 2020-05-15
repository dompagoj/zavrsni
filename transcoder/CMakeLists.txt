cmake_minimum_required(VERSION 3.15)
project(transcoder)
set(CMAKE_CXX_STANDARD 20)

set(CLANG_COMPILE_FLAGS "-fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_COMPILE_FLAGS}")

add_subdirectory(lib/FFmpeg)

include(sources.cmake)

add_executable(transcoder src/main.cpp ${SOURCES} )

target_link_libraries(transcoder FFmpeg)
