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
#include "libswscale/swscale.h"
}

int main()
{
  av_log_set_level(AV_LOG_DEBUG);
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

  av::FormatContext InputFormatCtx;
  InputFormatCtx.OpenInput(DeviceName, InputFormat, Opts);
  InputFormatCtx.Dump();

  auto RawVideoCodecParams = av::Utils::FindContextParamsFromFormatStreams(InputFormatCtx, av::MEDIA_TYPE::VIDEO);
  av::CodecContext InputVideoDecoder(RawVideoCodecParams);

  InputVideoDecoder.Open().Expect("Failed to open InputVideoDecoder");

  av::CodecContext H264Encoder(av::Codec(AV_CODEC_ID_H264, av::CodecType::ENCODER));

  auto Framerate = AVRational{30, 1};
  H264Encoder.Ptr->bit_rate = 300000;
  H264Encoder.Ptr->width = InputVideoDecoder.Ptr->width;
  H264Encoder.Ptr->height = InputVideoDecoder.Ptr->height;
  H264Encoder.Ptr->time_base = av_inv_q(Framerate);
  H264Encoder.Ptr->gop_size = 10;
  H264Encoder.Ptr->max_b_frames = 1;
  H264Encoder.Ptr->framerate = Framerate;
  H264Encoder.Ptr->pix_fmt = AV_PIX_FMT_YUV420P;
  H264Encoder.Ptr->codec_type = AVMEDIA_TYPE_VIDEO;
  H264Encoder.Ptr->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

  H264Encoder.Open().Expect("Failed to open H264Encoder");

  //  int PacketsToSend = 1;

  AVFrame* DecodedFrame = av_frame_alloc();
  auto Packet = InputFormatCtx.ReadFrame();

  printf("Sending packet for decoding! \n");
  int err = avcodec_send_packet(InputVideoDecoder.Ptr, &Packet.RawPacket);
  printf("Sending packet for decoding! done!: %d\n", err);

  printf("Receiving frame from decoder! \n");
  int ret = avcodec_receive_frame(InputVideoDecoder.Ptr, DecodedFrame);

  printf("%d", H264Encoder.Ptr->internal != nullptr);
  printf("Receiving frame from decoder! done!: %d %d \n", ret, DecodedFrame->width);

  SwsContext* SwsCtx = sws_getContext(640, 480, AV_PIX_FMT_YUYV422, 640, 480, AV_PIX_FMT_YUV420P, 0, 0, 0, 0);

  AVFrame* DstFrame = av_frame_alloc();
  DstFrame->format = AV_PIX_FMT_YUV420P;
  DstFrame->width = DecodedFrame->width;
  DstFrame->height = DecodedFrame->height;
  DstFrame->pts = DecodedFrame->pts;
  DstFrame->pkt_dts = DecodedFrame->pkt_dts;
  av_frame_get_buffer(DstFrame, 0);

  int ret1337 =
      sws_scale(SwsCtx, DecodedFrame->data, DecodedFrame->linesize, 0, 480, DstFrame->data, DstFrame->linesize);

  printf("yuhu, %d \n", ret1337);

  int err3 = avcodec_send_frame(H264Encoder.Ptr, DstFrame);
  err3 = avcodec_send_frame(H264Encoder.Ptr, DstFrame);
  err3 = avcodec_send_frame(H264Encoder.Ptr, DstFrame);
  printf("Sent frame to encoder! %d \n", err3);
  if (err3 != 0 && err3 != AVERROR(EAGAIN)) { exit(1); }

  AVPacket EncodedPacket;
  av_init_packet(&EncodedPacket);

  int ret2 = 0;
  int RetryCount = 0;
  while (ret2 >= 0 && RetryCount < 10)
  {
    printf("Receiving packet from encoder! \n");
    ret2 = avcodec_receive_packet(H264Encoder.Ptr, &EncodedPacket);
    printf("Receiving packet from encoder! done!: %d Decoded packet size:  %d\n", ret2, EncodedPacket.size);

    if (ret2 == AVERROR(EAGAIN))
    {
      ret2 = 0;
      RetryCount++;
    }
  }

  printf("Done !, encoded packet size:  %d \n", EncodedPacket.size);

  //  auto Packet = H264Encoder.ReceivePacket().Unwrap();
  av_frame_free(&DecodedFrame);
  av_frame_free(&DstFrame);
  av_packet_unref(&EncodedPacket);

  return 0;
}
