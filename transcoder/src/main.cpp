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
  av::Utils::RegisterAllDevices();

  auto InputFormat = av::Utils::FindInputFormat(Constants::GET_VIDEO_DRIVER()).Expect("Failed to find input format");

  auto DeviceName = av::Utils::FindInputDevice(InputFormat).Expect("Failed to find all input devices");

  if (!InputFormat)
  {
    printf("Input format not found!, format: %s \n", Constants::GET_VIDEO_DRIVER());
    exit(1);
  }

  av::Dictionary Opts;
  Opts.Set(av::Dictionary::Opts::FRAMERATE, "30");
  Opts.Set(av::Dictionary::Opts::VIDEO_SIZE, "640x480");

  av::FormatContext FormatContext;
  avformat_open_input(&FormatContext.Data(), DeviceName.c_str(), InputFormat, &*Opts);

  av::Utils::DumpContext(FormatContext);

  auto StreamCodec = av::Codec::FindFromAVStreams(FormatContext).Unwrap();
  auto StreamCodecContext = avcodec_alloc_context3(StreamCodec.Ptr);

  printf("%d", StreamCodecContext->codec_id);

  av::StackPacket Packet;
  int FramesProcessed = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  for (;;)
  {
    int err = FormatContext.ReadFrame(Packet);

    if (err < 0 || Packet.RawPacket.stream_index != StreamCodec.StreamIndex) continue;

    printf("Packet Size: %d \n", Packet.Size());
    Packet.Clear();
    //    FramesProcessed++;
  }
#pragma clang diagnostic pop

  avformat_close_input(&FormatContext.Data());

  return 0;
}
