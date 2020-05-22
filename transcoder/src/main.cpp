#include "libav_wrappers/Codec.h"
#include "libav_wrappers/CodecContext.h"
#include "libav_wrappers/Dictionary.h"
#include "libav_wrappers/FormatContext.h"
#include "libav_wrappers/Packet.h"
#include "libav_wrappers/Utils.h"

extern "C"
{
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
}

int err;

int main()
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
  FormatContext.OpenInput(DeviceName, InputFormat, Opts);
  FormatContext.Dump();

  auto VideoInputCodec = av::Codec::FindFromAVStreams(FormatContext, av::CodecType::VIDEO).Unwrap();
  //  av::CodecContext InputCodecContext(VideoInputContext);

  av::Codec H264Codec(AV_CODEC_ID_H264);
  av::CodecContext H264Encoder(H264Codec);

  H264Encoder.Ptr->width = 1280;
  H264Encoder.Ptr->height = 720;
  H264Encoder.Ptr->time_base = AVRational{1, 30};
  H264Encoder.Ptr->pix_fmt = AV_PIX_FMT_YUV422P;

  H264Encoder.Open();

  printf("H264Codec name: %s \n", H264Codec.Ptr->long_name);
  printf("H264Encoder not null: %s \n", H264Encoder.Ptr != nullptr ? "True" : "False");

  int PacketsToSend = 1;

  printf("Sending packets \n");
  for (int i = 0; i < PacketsToSend; i++)
  {
    auto Packet = FormatContext.ReadFrame();

    if (Packet.RawPacket.stream_index != VideoInputCodec.StreamIndex) continue;

    auto Result = H264Encoder.SendPacket(Packet);
    if (Result.HasError()) { printf("Error while sending packet %s", Result.Err().Msg.c_str()); }
  }

  return 0;

  printf("Receiving decoded packets \n");
  av::StackPacket EncodecPacket;
  while (int rErr = H264Encoder.ReceivePacket(EncodecPacket) != AVERROR_EOF)
  {
    if (rErr < 0)
    {
      printf("Got error! \n");
      continue;
      ;
    }
    printf("Encoded Packet Size: %d \n", EncodecPacket.RawPacket.size);
  }

  return 0;
}
