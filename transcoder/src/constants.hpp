#pragma once

namespace Constants
{
constexpr const char *const VIDEO_DRIVER_LINUX = "v4l2";
constexpr const char *const VIDEO_DRIVER_WINDOWS = "dshow";
constexpr const char *const VIDEO_DRIVER_OSX = "avfoundation";

constexpr const char *GET_VIDEO_DRIVER()
{
#ifdef linux
  return VIDEO_DRIVER_LINUX;
#endif

#ifdef __APPLE__
  return VIDEO_DRIVER_OSX;
#endif

#ifdef __WIN32
  return VIDEO_DRIVER_WINDOWS;
#endif
}
} // namespace AV
