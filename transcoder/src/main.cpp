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
const static int FRAMERATE = 30;

int main()
{
  av_log_set_level(AV_LOG_DEBUG);
  av::Utils::RegisterAllDevices();

  auto InputFormat = av::Utils::FindInputFormat(Constants::GET_VIDEO_DRIVER())
      .Expect("Failed to find input format");
  auto DeviceName = av::Utils::FindInputDevice(InputFormat).Expect("Failed to find all input devices");

  av::Dictionary Opts;
  Opts.Set(av::Dictionary::Opts::FRAMERATE, FRAMERATE);
  Opts.Set(av::Dictionary::Opts::VIDEO_SIZE, av::Utils::GetResolutionString(WIDTH, HEIGHT));

  av::FormatContext InputFormatCtx;
  InputFormatCtx.Ptr->flags |= AVFMT_FLAG_NOBUFFER;
  InputFormatCtx.OpenInput(DeviceName, InputFormat, Opts);
  InputFormatCtx.Dump();

  auto InputVideoStream = av::Utils::FindStreamFromFormatCtx(InputFormatCtx, av::MEDIA_TYPE::VIDEO);
  InputVideoStream->start_time = 0;

  auto FramerateRational = InputVideoStream->r_frame_rate;
  auto TimebaseRational = av_inv_q(FramerateRational);
  WIDTH = InputVideoStream->codecpar->width;
  HEIGHT = InputVideoStream->codecpar->height;

  av::CodecContext InputVideoDecoder(InputVideoStream->codecpar, av::CodecType::DECODER);
  InputVideoDecoder.Ptr->rc_buffer_size = 0;
  InputVideoDecoder.Ptr->delay = 0;

  InputVideoDecoder.Open().Expect("Failed to open InputVideoDecoder");

  av::CodecContext CodecEncoder(av::Codec(AV_CODEC_ID_VP9, av::CodecType::ENCODER));
//  CodecEncoder.Ptr->me_cmp = 1;
//  CodecEncoder.Ptr->me_range = 16;
//  CodecEncoder.Ptr->me_subpel_quality = 5;
//  CodecEncoder.Ptr->max_qdiff = 4;
//  CodecEncoder.Ptr->qcompress = 0.6;
//  CodecEncoder.Ptr->gop_size = 10;
//  CodecEncoder.Ptr->max_b_frames = 1;
  CodecEncoder.Ptr->bit_rate = 1800 * 1000;
  CodecEncoder.Ptr->qmin = 4;
  CodecEncoder.Ptr->qmax = 48;
  CodecEncoder.Ptr->delay = 0;
  CodecEncoder.Ptr->width = InputVideoDecoder.Ptr->width;
  CodecEncoder.Ptr->height = InputVideoDecoder.Ptr->height;
  CodecEncoder.Ptr->time_base = TimebaseRational;
  CodecEncoder.Ptr->framerate = FramerateRational;
  CodecEncoder.Ptr->thread_count = 8;
  CodecEncoder.Ptr->pix_fmt = AV_PIX_FMT_YUV420P;
//  CodecEncoder.Ptr->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  CodecEncoder.Ptr->flags2 |= AV_CODEC_FLAG2_LOCAL_HEADER | AV_CODEC_FLAG2_SHOW_ALL;

//  av_opt_set(CodecEncoder.Ptr->priv_data, "preset", "ultrafast", 0);
//  av_opt_set(CodecEncoder.Ptr->priv_data, "tune", "zerolatency", 0);

  av_opt_set(CodecEncoder.Ptr->priv_data, "b:v", "1800k", 0);
//  av_opt_set_int(CodecEncoder.Ptr->priv_data, "crf", 33, 0);
  av_opt_set(CodecEncoder.Ptr->priv_data, "quality", "realtime", 0);
  av_opt_set_int(CodecEncoder.Ptr->priv_data, "speed", 6, 0);
  av_opt_set_int(CodecEncoder.Ptr->priv_data, "lag-in-frames", 0, 0);
  av_opt_set_int(CodecEncoder.Ptr->priv_data, "frame-parallel", 1, 0);
  av_opt_set_int(CodecEncoder.Ptr->priv_data, "tile-columns", 4, 0);
  av_opt_set_int(CodecEncoder.Ptr->priv_data, "threads", 4, 0);
  av_opt_set_int(CodecEncoder.Ptr->priv_data, "static-thresh", 0, 0);
  av_opt_set_int(CodecEncoder.Ptr->priv_data, "max-intra-rate", 300, 0);
  av_opt_set_int(CodecEncoder.Ptr->priv_data, "row-mt", 1, 0);

  CodecEncoder.Open().Expect("Failed to open H264Encoder");
  av::FormatContext OutputFormatCtx;
  avformat_alloc_output_context2(&OutputFormatCtx.Ptr, nullptr, "webm", nullptr);
  OutputFormatCtx.Ptr->oformat->video_codec = CodecEncoder.Codec->id;
  OutputFormatCtx.Ptr->flags |= AVFMT_FLAG_NOBUFFER | AVFMT_FLAG_FLUSH_PACKETS;
  OutputFormatCtx.Ptr->ctx_flags |= AVFMTCTX_NOHEADER;
  OutputFormatCtx.Ptr->max_analyze_duration = 2147483647;
  OutputFormatCtx.Ptr->probesize = 2147483647;

  auto OutputVideoStream = avformat_new_stream(OutputFormatCtx.Ptr, CodecEncoder.Codec);
  avcodec_parameters_from_context(OutputVideoStream->codecpar, CodecEncoder.Ptr);
  OutputVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  OutputVideoStream->time_base = TimebaseRational;
  OutputVideoStream->r_frame_rate = FramerateRational;

  int file_open_err = avio_open2(&OutputFormatCtx.Ptr->pb, "tcp://localhost:13000", AVIO_FLAG_WRITE, nullptr, nullptr);
  if (file_open_err < 0)
  {
    printf("Failed to open file! %d \n", file_open_err);
    exit(1);
  }

  av::Dictionary OutputOpts;
  OutputOpts.Set("live", "1");
  OutputFormatCtx.WriteHeader(OutputOpts).Unwrap();
  AVFrame* DecodedFrame = av_frame_alloc();
  AVFrame* DstFrame = av_frame_alloc();
  SwsContext* SwsCtx = sws_getContext(WIDTH, HEIGHT, InputVideoDecoder.Ptr->pix_fmt, WIDTH, HEIGHT,
                                      CodecEncoder.Ptr->pix_fmt, 0, 0, 0, 0);

  int64_t LatestTimestamp = 0;
  for (int64_t i = 0;; i++)
  {
    auto InputVideoPacket = InputFormatCtx.ReadFrame();
    avcodec_send_packet(InputVideoDecoder.Ptr, &InputVideoPacket.RawPacket);

    avcodec_receive_frame(InputVideoDecoder.Ptr, DecodedFrame);

    av_frame_copy_props(DstFrame, DecodedFrame);
    DstFrame->format = CodecEncoder.Ptr->pix_fmt;
    DstFrame->width = DecodedFrame->width;
    DstFrame->height = DecodedFrame->height;
    av_frame_get_buffer(DstFrame, 0);

    sws_scale(SwsCtx, DecodedFrame->data, DecodedFrame->linesize, 0, HEIGHT, DstFrame->data, DstFrame->linesize);

    avcodec_send_frame(CodecEncoder.Ptr, DstFrame);

    av_frame_unref(DecodedFrame);

    int ret = 0;
    av::StackPacket EncodedPacket;
    EncodedPacket.RawPacket.stream_index = OutputVideoStream->index;
    av_packet_make_refcounted(&EncodedPacket.RawPacket);

    while (ret >= 0)
    {
      ret = CodecEncoder.ReceivePacket(EncodedPacket);
//      EncodedPacket.RawPacket.stream_index = OutputVideoStream->index;
//      EncodedPacket.RawPacket.flags |= AV_PKT_FLAG_KEY;

      EncodedPacket.RawPacket.pts = LatestTimestamp;
      EncodedPacket.RawPacket.dts = LatestTimestamp;
      EncodedPacket.RawPacket.duration =
          av_rescale_q(InputVideoPacket.RawPacket.duration, InputVideoStream->time_base, OutputVideoStream->time_base);
      LatestTimestamp += EncodedPacket.RawPacket.duration;
      EncodedPacket.RawPacket.pos = -1;

      if (ret == 0)
      {
        int write_err = av_interleaved_write_frame(OutputFormatCtx.Ptr, &EncodedPacket.RawPacket);
        if (write_err < 0)
        {
          av::Error write_err2(write_err);
          printf("Failed to write frame! %d %s \n", write_err, write_err2.Msg.c_str());
          exit(1);
        }
      }
    }
    av_frame_unref(DstFrame);
  }

  av_write_trailer(OutputFormatCtx.Ptr);
  av_frame_free(&DecodedFrame);
  av_frame_free(&DstFrame);
  avio_close(OutputFormatCtx.Ptr->pb);
//  avformat_close_input(&OutputFormatCtx);

  return 0;
}
