#include "libav_wrappers/AVPacket.h"
#include <iostream>

extern "C"
{
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
}

int main()
{
  avdevice_register_all();

  AVFormatContext *FormatContext = avformat_alloc_context();

  constexpr auto FileName = "/dev/video0";

  auto format = "v4l2";
  auto InputFormat = av_find_input_format(format);

  if (!InputFormat) std::cout << "Input format null!";

  bool test = false;

  avformat_free_context(FormatContext);

  return 0;
}
