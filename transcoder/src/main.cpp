#include "libav_wrappers/Codec.h"
#include "libav_wrappers/CodecContext.h"
#include "libav_wrappers/Utils.h"
#include "network.hpp"
#include <cassert>

extern "C"
{
#include "libavdevice/avdevice.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

static int WIDTH = 800;
static int HEIGHT = 600;
const static int FRAMERATE = 25;

int main()
{
  av_log_set_level(AV_LOG_DEBUG);
  av::Utils::RegisterAllDevices();

  auto InputFormat = av::Utils::FindInputFormat(Constants::GET_VIDEO_DRIVER())
      .Expect("Failed to find input format");
  auto DeviceName = av::Utils::FindInputDevice(InputFormat).Expect("Failed to find all input devices");

  if (!InputFormat)
  {
    printf("Input format not found!, format: %s \n", Constants::GET_VIDEO_DRIVER());
    exit(1);
  }

  av::Dictionary Opts;
  Opts.Set(av::Dictionary::Opts::FRAMERATE, FRAMERATE);
  Opts.Set(av::Dictionary::Opts::VIDEO_SIZE, av::Utils::GetResolutionString(WIDTH, HEIGHT));

  av::FormatContext InputFormatCtx;
  InputFormatCtx.OpenInput(DeviceName, InputFormat, Opts);
  InputFormatCtx.Dump();

  auto InputVideoStream = av::Utils::FindStreamFromFormatCtx(InputFormatCtx, av::MEDIA_TYPE::VIDEO);
  InputVideoStream->start_time = 0;
  auto FramerateRational = InputVideoStream->avg_frame_rate;
  auto TimebaseRational = av_inv_q(FramerateRational);
  WIDTH = InputVideoStream->codecpar->width;
  HEIGHT = InputVideoStream->codecpar->height;

  av::CodecContext InputVideoDecoder(InputVideoStream->codecpar, av::CodecType::DECODER);
  InputVideoDecoder.Ptr->rc_buffer_size = 0;
  InputVideoDecoder.Ptr->delay = 0;

  InputVideoDecoder.Open().Expect("Failed to open InputVideoDecoder");

  av::CodecContext H264Encoder(av::Codec(AV_CODEC_ID_H264, av::CodecType::ENCODER));

//  H264Encoder.Ptr->bit_rate = 1000000;
  H264Encoder.Ptr->rc_buffer_size = 0;
  H264Encoder.Ptr->me_cmp = 1;
  H264Encoder.Ptr->me_range = 16;
  H264Encoder.Ptr->qmin = 10;
  H264Encoder.Ptr->qmax = 51;
  H264Encoder.Ptr->me_subpel_quality = 5;
  H264Encoder.Ptr->max_qdiff = 4;
  H264Encoder.Ptr->delay = 0;
  H264Encoder.Ptr->qcompress = 0.6;
  H264Encoder.Ptr->width = InputVideoDecoder.Ptr->width;
  H264Encoder.Ptr->height = InputVideoDecoder.Ptr->height;
  H264Encoder.Ptr->time_base = TimebaseRational;
  H264Encoder.Ptr->framerate = FramerateRational;
  H264Encoder.Ptr->gop_size = 10;
  H264Encoder.Ptr->max_b_frames = 1;
  H264Encoder.Ptr->pix_fmt = AV_PIX_FMT_YUV420P;
  H264Encoder.Ptr->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  av_opt_set(H264Encoder.Ptr->priv_data, "preset", "ultrafast", 0);
  av_opt_set_int(H264Encoder.Ptr->priv_data, "crf", 31, 0);
  av_opt_set(H264Encoder.Ptr->priv_data, "tune", "zerolatency", 0);

  H264Encoder.Open().Expect("Failed to open H264Encoder");
  AVFormatContext* OutputFormatCtx = avformat_alloc_context();
  avformat_alloc_output_context2(&OutputFormatCtx, av_guess_format("MP4", nullptr, nullptr), nullptr, nullptr);
  OutputFormatCtx->oformat->video_codec = H264Encoder.Codec->id;

  auto OutputVideoStream = avformat_new_stream(OutputFormatCtx, H264Encoder.Codec);
  avcodec_parameters_from_context(OutputVideoStream->codecpar, H264Encoder.Ptr);
  OutputVideoStream->time_base = TimebaseRational;
  OutputVideoStream->avg_frame_rate = FramerateRational;
  OutputVideoStream->start_time = 0;

  av_dump_format(OutputFormatCtx, 0, nullptr, 1);

  int file_open_err = avio_open2(&OutputFormatCtx->pb, "tcp://localhost:13000", AVIO_FLAG_WRITE, nullptr, nullptr);
  if (file_open_err < 0)
  {
    printf("Failed to open file! %d \n", file_open_err);
    exit(1);
  }

  av::Dictionary OutputOpts;
  OutputOpts.Set("live", "1");
  OutputOpts.Set("movflags", "frag_keyframe+empty_moov+default_base_moof");

  int write_header_err = avformat_write_header(OutputFormatCtx, &OutputOpts.Ptr);
  if (write_header_err < 0)
  {
    printf("failed to write header %d \n", write_header_err);
    exit(1);
  }

  AVFrame* DecodedFrame = av_frame_alloc();
  AVFrame* DstFrame = av_frame_alloc();
  SwsContext* SwsCtx = sws_getContext(WIDTH, HEIGHT, InputVideoDecoder.Ptr->pix_fmt, WIDTH, HEIGHT,
                                      H264Encoder.Ptr->pix_fmt, 0, 0, 0, 0);

//  int FramesToProcess = 200;
  int64_t LatestTimestamp = 0;
  for (int64_t i = 0;; i++)
  {
    auto InputVideoPacket = InputFormatCtx.ReadFrame();
    avcodec_send_packet(InputVideoDecoder.Ptr, &InputVideoPacket.RawPacket);

    avcodec_receive_frame(InputVideoDecoder.Ptr, DecodedFrame);

    av_frame_copy_props(DstFrame, DecodedFrame);
    DstFrame->format = H264Encoder.Ptr->pix_fmt;
    DstFrame->width = DecodedFrame->width;
    DstFrame->height = DecodedFrame->height;
    av_frame_get_buffer(DstFrame, 0);

    sws_scale(SwsCtx, DecodedFrame->data, DecodedFrame->linesize, 0, HEIGHT, DstFrame->data, DstFrame->linesize);

    int err = avcodec_send_frame(H264Encoder.Ptr, DstFrame);

    if (err != 0 && err != AVERROR(EAGAIN)) { exit(1); }

    av_frame_unref(DecodedFrame);

    int ret2 = 0;
    av::StackPacket EncodedPacket;
    av_packet_make_refcounted(&EncodedPacket.RawPacket);

//    if (FramesToProcess - i == 1) { avcodec_send_frame(H264Encoder.Ptr, nullptr); }

    while (ret2 >= 0)
    {
      ret2 = H264Encoder.ReceivePacket(EncodedPacket);
      EncodedPacket.RawPacket.stream_index = OutputVideoStream->index;
      EncodedPacket.RawPacket.flags |= AV_PKT_FLAG_KEY;

      EncodedPacket.RawPacket.pts = LatestTimestamp;
      EncodedPacket.RawPacket.dts = LatestTimestamp;
      EncodedPacket.RawPacket.duration =
          av_rescale_q(InputVideoPacket.RawPacket.duration, InputVideoStream->time_base, OutputVideoStream->time_base);
      LatestTimestamp += EncodedPacket.RawPacket.duration;
      EncodedPacket.RawPacket.pos = -1;

      if (ret2 == 0)
      {

        int write_err = av_interleaved_write_frame(OutputFormatCtx, &EncodedPacket.RawPacket);
        if (write_err < 0)
        {
          printf("Failed to write frame! %d \n", write_err);
          exit(1);
        }
      }
    }
    av_frame_unref(DstFrame);
  }

  av_write_trailer(OutputFormatCtx);

  av_frame_free(&DecodedFrame);
  av_frame_free(&DstFrame);
  avio_close(OutputFormatCtx->pb);
  avformat_free_context(OutputFormatCtx);
//  avformat_close_input(&OutputFormatCtx);

  return 0;
}
