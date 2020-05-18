cmake_minimum_required(VERSION 3.15)
project(transcoder)
set(CMAKE_CXX_STANDARD 20)

include(project_settings.cmake)

set(CLANG_COMPILE_FLAGS "-fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_COMPILE_FLAGS}")

add_compile_definitions(CXXOPTS_NO_EXCEPTIONS)

add_subdirectory(lib/FFmpeg)

include(sources.cmake)

add_executable(transcoder src/main.cpp ${SOURCES})

target_include_directories(transcoder PUBLIC /usr/include/node)
target_link_libraries(transcoder FFmpeg)
