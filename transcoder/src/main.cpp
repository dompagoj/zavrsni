extern "C"
{
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
}

#include "constants.hpp"
#include "libav_wrappers/Dictionary.h"
#include "libav_wrappers/FormatContext.h"
#include "libav_wrappers/Packet.h"

int main()
{
  avdevice_register_all();

  AV::FormatContext Context;
  auto InputFormat = av_find_input_format(Constants::GET_VIDEO_DRIVER());

  // auto Devices = AV::Utils::FindAllInputDevices(InputFormat);

  auto FileName = "/dev/video0";

  if (!InputFormat)
  {
    printf("Input format not found!, format: %s \n", Constants::GET_VIDEO_DRIVER());
    return 1;
  }

  AV::Dictionary Opts;

  Opts.Set("framerate", "30");
  Opts.Set("video_size", "640x480");

  avformat_open_input(Context.DataPtr(), FileName, InputFormat, Opts.DataPtr());

  av_dump_format(*Context, 0, FileName, 0);

  while (true)
  {
    AV::Packet Packet;
    int err = av_read_frame(Context.Data(), *Packet);
    if (err) return 1;

    printf("Packet info: %ld \n", Packet.RawPacket->pts);
  }

  avformat_close_input(Context.DataPtr());

  return 0;
}
