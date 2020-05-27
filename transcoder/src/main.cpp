#include "libav_wrappers/Codec.h"
#include "libav_wrappers/CodecContext.h"
#include "libav_wrappers/Dictionary.h"
#include "libav_wrappers/FormatContext.h"
#include "libav_wrappers/Packet.h"
#include "libav_wrappers/Utils.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

int custom_io_write(void* opaque, uint8_t* buffer, int32_t buffer_size)
{
  if (!opaque) { printf("wut"); }
  printf("Custom io write called! %s, %d", buffer, buffer_size);

  return 0;
}

int64_t custom_io_seek(void* opaque, int64_t offset, int whence)
{
  if (!opaque) { printf("wut"); }
  printf("Custom io write called! %ld, %d", offset, whence);

  return 0;
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

  auto Framerate = AVRational{30, 1};
  auto Timebase = AVRational{1, 30};
  auto RawVideoCodecParams = av::Utils::FindContextParamsFromFormatStreams(InputFormatCtx, av::MEDIA_TYPE::VIDEO);
  av::CodecContext InputVideoDecoder(RawVideoCodecParams, av::CodecType::DECODER);
  InputVideoDecoder.Ptr->framerate = Framerate;
  InputVideoDecoder.Ptr->time_base = Timebase;

  InputVideoDecoder.Open().Expect("Failed to open InputVideoDecoder");

  av::CodecContext H264Encoder(av::Codec(AV_CODEC_ID_H264, av::CodecType::ENCODER));

  H264Encoder.Ptr->bit_rate = 300000;
  H264Encoder.Ptr->width = InputVideoDecoder.Ptr->width;
  H264Encoder.Ptr->height = InputVideoDecoder.Ptr->height;
  H264Encoder.Ptr->time_base = InputVideoDecoder.Ptr->time_base;
  H264Encoder.Ptr->gop_size = 10;
  H264Encoder.Ptr->max_b_frames = 1;
  H264Encoder.Ptr->framerate = InputVideoDecoder.Ptr->framerate;
  H264Encoder.Ptr->pix_fmt = AV_PIX_FMT_YUV420P;
  H264Encoder.Ptr->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  av_opt_set(H264Encoder.Ptr->priv_data, "preset", "ultrafast", 0);

  H264Encoder.Open().Expect("Failed to open H264Encoder");

  AVFormatContext* OutputFormatCtx = avformat_alloc_context();
  OutputFormatCtx->oformat = av_guess_format("MP4", "test.mp4", nullptr);

  AVCodecParameters* VideoStreamCodecParams = avcodec_parameters_alloc();
  avcodec_parameters_from_context(VideoStreamCodecParams, H264Encoder.Ptr);
  auto VideoStream = avformat_new_stream(OutputFormatCtx, nullptr);
  VideoStream->avg_frame_rate = InputVideoDecoder.Ptr->framerate;
  avcodec_parameters_from_context(VideoStream->codecpar, H264Encoder.Ptr);
  VideoStream->time_base = InputVideoDecoder.Ptr->time_base;

  int BufferSize = 4000;
  unsigned char* AvioBuffer = (unsigned char*)av_malloc(BufferSize);

  AVIOContext* AvioCtx = avio_alloc_context(AvioBuffer, BufferSize, 1, (void*)42, nullptr, nullptr, &custom_io_seek);
  OutputFormatCtx->pb = AvioCtx;

  av::Dictionary OutputOpts;
  OutputOpts.Set("live", "1");

  av_dump_format(OutputFormatCtx, 0, nullptr, 1);

  int file_open_err = avio_open2(&OutputFormatCtx->pb, "./test.mp4", AVIO_FLAG_READ_WRITE, nullptr, nullptr);

  if (file_open_err < 0)
  {
    printf("Failed to open file! %d \n", file_open_err);
    exit(1);
  }

  int write_header_err = avformat_write_header(OutputFormatCtx, &OutputOpts.Ptr);
  if (write_header_err < 0)
  {
    printf("failed to write header %d \n", write_header_err);
    exit(1);
  }

  //  av_write_trailer(OutputFormatCtx);

  AVFrame* DecodedFrame = av_frame_alloc();
  AVFrame* DstFrame = av_frame_alloc();
  SwsContext* SwsCtx =
      sws_getContext(640, 480, InputVideoDecoder.Ptr->pix_fmt, 640, 480, H264Encoder.Ptr->pix_fmt, 0, 0, 0, 0);

  int FramesToProcess = 1000;
  printf("Encoding 1000 frames! \n");
  for (int i = 0; i < FramesToProcess; i++)
  {
    auto InputVideoPacket = InputFormatCtx.ReadFrame();
    printf("Sending packet for decoding! \n");
    int err = avcodec_send_packet(InputVideoDecoder.Ptr, &InputVideoPacket.RawPacket);
    printf("Sending packet for decoding! done!: %d\n", err);

    printf("Receiving frame from decoder! \n");
    int ret = avcodec_receive_frame(InputVideoDecoder.Ptr, DecodedFrame);

    printf("%d", H264Encoder.Ptr->internal != nullptr);
    printf("Receiving frame from decoder! done!: %d %d \n", ret, DecodedFrame->width);

    av_frame_copy_props(DstFrame, DecodedFrame);
    DstFrame->format = H264Encoder.Ptr->pix_fmt;
    DstFrame->width = DecodedFrame->width;
    DstFrame->height = DecodedFrame->height;
    DstFrame->pts = i * (15873) / 30;
    DstFrame->pkt_dts = DecodedFrame->pkt_dts;
    av_frame_get_buffer(DstFrame, 0);

    sws_scale(SwsCtx, DecodedFrame->data, DecodedFrame->linesize, 0, 480, DstFrame->data, DstFrame->linesize);

    int err3 = avcodec_send_frame(H264Encoder.Ptr, DstFrame);

    printf("Sent frame to encoder! %d i: %d\n", err3, i);
    if (err3 != 0 && err3 != AVERROR(EAGAIN)) { exit(1); }

    av_frame_unref(DecodedFrame);
    int ret2 = 0;
    printf("Receiving encoded packets! \n");
    av::StackPacket EncodedPacket;
    av_packet_make_refcounted(&EncodedPacket.RawPacket);

    while (ret2 >= 0 && ret2 != AVERROR_EOF)
    {
      ret2 = H264Encoder.ReceivePacket(EncodedPacket);

      if (ret2 == 0)
      {
        printf("Encoded Packet size: %d \n", EncodedPacket.RawPacket.size);
        EncodedPacket.RawPacket.stream_index = VideoStream->index;
        int write_err = av_interleaved_write_frame(OutputFormatCtx, &EncodedPacket.RawPacket);
        if (write_err < 0)
        {
          printf("Failed to write frame! %d \n", write_err);
          exit(1);
        }
      }
    }
  }

  av_write_trailer(OutputFormatCtx);

  avformat_close_input(&OutputFormatCtx);
  av_frame_free(&DecodedFrame);
  av_frame_free(&DstFrame);

  return 0;
}
