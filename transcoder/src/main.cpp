#include "libav_wrappers/Codec.h"
#include "libav_wrappers/Dictionary.h"
#include "libav_wrappers/FormatContext.h"
#include "libav_wrappers/Packet.h"
#include "libav_wrappers/Utils.h"

extern "C"
{
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
}

int main(int argc, char** argv)
{
  AV::Utils::RegisterAllDevices();

  auto InputFormat = AV::Utils::FindInputFormat(Constants::GET_VIDEO_DRIVER()).Expect("Failed to find input format");

  auto Devices = AV::Utils::FindAllInputDevices(InputFormat).Expect("Failed to find all input devices");

  auto FileName = "/dev/video0";

  if (!InputFormat)
  {
    printf("Input format not found!, format: %s \n", Constants::GET_VIDEO_DRIVER());
    exit(1);
  }

  AV::Dictionary Opts;

  Opts.Set(AV::Dictionary::Opts::FRAMERATE, "30");
  Opts.Set(AV::Dictionary::Opts::VIDEO_SIZE, "640x480");

  AV::FormatContext FormatContext;
  avformat_open_input(&FormatContext.Data(), FileName, InputFormat, &Opts.Ptr);

  AV::Utils::DumpContext(FormatContext);

  auto StreamCodec = AV::Codec::FindFromAVStreams(FormatContext).Unwrap();

  //  AV::StackPacket Packet;
  int FramesProcessed = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  for (;;)
  {
    auto Packet = FormatContext.ReadFrame();

    if (Packet.IsEmpty() || Packet.RawPacket.stream_index != StreamCodec.StreamIndex) { continue; }

    printf("Packet Size: %d \n", Packet.RawPacket.size);
    //    FramesProcessed++;
  }
#pragma clang diagnostic pop

  avformat_close_input(&FormatContext.Data());

  return 0;
}
