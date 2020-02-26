cmake_minimum_required(VERSION 3.15)
project(libav_test)

set(CMAKE_CXX_STANDARD 17)

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

add_executable(libav_test main.cpp)

target_link_libraries(libav_test PUBLIC ${AVUTIL_LIBRARY} ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVDEVICE_LIBRARY})
target_include_directories(libav_test PUBLIC ${AVCODEC_INCLUDE_DIR} ${AVCODEC_LIBRARY} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR})
