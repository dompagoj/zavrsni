cmake_minimum_required(VERSION 3.15)
project(transcoder)
set(CMAKE_CXX_STANDARD 20)


add_subdirectory(lib/FFmpeg)
add_subdirectory(lib/nlohmann_json)
include(sources.cmake)

add_executable(transcoder src/main.cpp ${SOURCES})

include(project_settings.cmake)
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

#target_link_libraries(transcoder${CONAN_LIBS})
target_link_libraries(transcoder FFmpeg nlohmann_json::nlohmann_json)
