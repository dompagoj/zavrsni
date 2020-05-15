#include "constants.hpp"
#include "libav_wrappers/Dictionary.h"
#include "libav_wrappers/FormatContext.h"
#include "libav_wrappers/Packet.h"
#include "libav_wrappers/Utils.h"

extern "C"
{
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
}

int main()
{
  avdevice_register_all();

  AV::FormatContext Context;
  auto InputFormat = av_find_input_format(Constants::GET_VIDEO_DRIVER());

  auto Devices = AV::Utils::FindAllInputDevices(InputFormat);

  auto FileName = "/dev/video4";

  if (!InputFormat)
  {
    printf("Input format not found!, format: %s \n", Constants::GET_VIDEO_DRIVER());
    return 1;
  }

  AV::Dictionary Opts;

  Opts.Set("framerate", "30");
  Opts.Set("video_size", "1280x720");

  avformat_open_input(&Context.Data(), FileName, InputFormat, &Opts.Data());

  av_dump_format(Context.Data(), 0, FileName, 0);

  while (true)
  {
    AV::Packet Packet;
    int err = av_read_frame(Context.Data(), &Packet.GetInstance());
    if (err) return 1;

    printf("Packet info: %ld \n", Packet.GetInstance().pts);
  }

  avformat_close_input(&Context.Data());

  return 0;
}
