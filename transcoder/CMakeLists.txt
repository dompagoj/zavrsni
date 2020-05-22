cmake_minimum_required(VERSION 3.15)
project(transcoder)
set(CMAKE_CXX_STANDARD 20)


add_subdirectory(lib/FFmpeg)
include(sources.cmake)

add_executable(transcoder src/main.cpp ${SOURCES})

include(project_settings.cmake)
target_link_libraries(transcoder FFmpeg)
