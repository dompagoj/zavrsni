#include "libav_wrappers/Codec.h"
#include "libav_wrappers/CodecContext.h"
#include "libav_wrappers/Utils.h"
#include "Configuration.h"
#include "Utils.h"

extern "C"
{
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

int main()
{
  av_log_set_level(AV_LOG_DEBUG);
  av::Utils::RegisterAllDevices();

  auto Conf = Utils::LoadConfiguration();

  AVInputFormat* InputFormat = av::Utils::FindInputFormat(Constants::GET_VIDEO_DRIVER())
      .Expect("Failed to find input format");
  auto DeviceName = av::Utils::FindInputDevice(InputFormat).Expect("Failed to find all input devices");

  av::Dictionary Opts;
  Opts.Set(av::Dictionary::Opts::FRAMERATE, Conf.Framerate);
  Opts.Set(av::Dictionary::Opts::VIDEO_SIZE, av::Utils::GetResolutionString(Conf.Width, Conf.Height));

  av::FormatContext InputFormatCtx;
  InputFormatCtx.OpenInput(DeviceName, InputFormat, Opts);
  InputFormatCtx.Dump();

  auto InputVideoStream = av::Utils::FindStreamFromFormatCtx(InputFormatCtx, av::MEDIA_TYPE::VIDEO);
  InputVideoStream->start_time = 0;

  auto FramerateRational = InputVideoStream->r_frame_rate;
  auto TimebaseRational = av_inv_q(FramerateRational);
  Conf.Width = InputVideoStream->codecpar->width;
  Conf.Height = InputVideoStream->codecpar->height;

  av::CodecContext InputVideoDecoder(InputVideoStream->codecpar, av::CodecType::DECODER);
  InputVideoDecoder.Ptr->rc_buffer_size = 0;
  InputVideoDecoder.Ptr->delay = 0;

  InputVideoDecoder.Open().Expect("Failed to open InputVideoDecoder");

  av::CodecContext CodecEncoder(av::Codec(AV_CODEC_ID_VP9, av::CodecType::ENCODER));

  CodecEncoder.Ptr->max_b_frames = 0;
  CodecEncoder.Ptr->bit_rate = 1800 * 1000;
  CodecEncoder.Ptr->qmin = 4;
  CodecEncoder.Ptr->qmax = 48;
  CodecEncoder.Ptr->delay = 0;
  CodecEncoder.Ptr->width = InputVideoDecoder.Ptr->width;
  CodecEncoder.Ptr->height = InputVideoDecoder.Ptr->height;
  CodecEncoder.Ptr->time_base = TimebaseRational;
  CodecEncoder.Ptr->thread_count = 1;
  CodecEncoder.Ptr->pix_fmt = AV_PIX_FMT_YUV420P;
  CodecEncoder.Ptr->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  CodecEncoder.Ptr->flags2 |= AV_CODEC_FLAG2_SHOW_ALL;

//  av_opt_set(CodecEncoder.Ptr->priv_data, "preset", "ultrafast", 0);
//  av_opt_set(CodecEncoder.Ptr->priv_data, "tune", "zerolatency", 0);

  CodecEncoder.SetPrivData("quality", "realtime");
  CodecEncoder.SetPrivData("crf", 33);
  CodecEncoder.SetPrivData("b:v", 0);
  CodecEncoder.SetPrivData("lag-in-frames", 0);
  CodecEncoder.SetPrivData("frame-parallel", 1);
  CodecEncoder.SetPrivData("tile-columns", 4);
  CodecEncoder.SetPrivData("threads", 1);
  CodecEncoder.SetPrivData("static-thresh", 0);
  CodecEncoder.SetPrivData("max-intra-rate", 300);
  CodecEncoder.SetPrivData("row-mt", 1);

  CodecEncoder.Open().Expect("Failed to open H264Encoder");

  av::FormatContext OutputFormatCtx;
  OutputFormatCtx.AllocOutput("WebM");

  OutputFormatCtx.Ptr->oformat->video_codec = CodecEncoder.Codec->id;
  OutputFormatCtx.Ptr->flags |= AVFMT_FLAG_NOBUFFER;
  OutputFormatCtx.Ptr->max_analyze_duration = 2147483647;
  OutputFormatCtx.Ptr->probesize = 2147483647;
  OutputFormatCtx.Ptr->bit_rate = 1800 * 1000;

  auto OutputVideoStream = avformat_new_stream(OutputFormatCtx.Ptr, CodecEncoder.Codec);
  avcodec_parameters_from_context(OutputVideoStream->codecpar, CodecEncoder.Ptr);
  OutputVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  OutputVideoStream->avg_frame_rate = FramerateRational;

  av::Error NetworkError = avio_open2(&OutputFormatCtx.Ptr->pb, "tcp://localhost:13000", AVIO_FLAG_WRITE, nullptr,
                                      nullptr);
  if (NetworkError)
  {
    printf("Failed to establish connection! %s \n", NetworkError.Msg.c_str());
    exit(1);
  }

  av::Dictionary OutputOpts;
  OutputOpts.Set("live", "1");
  OutputFormatCtx.WriteHeader(OutputOpts).Unwrap();
  av::Frame DecodedFrame;
  av::Frame DstFrame;
  SwsContext* SwsCtx = sws_getContext(Conf.Width, Conf.Height, InputVideoDecoder.Ptr->pix_fmt, Conf.Width, Conf.Height,
                                      CodecEncoder.Ptr->pix_fmt, 0, 0, 0, 0);

  for (int64_t i = 0;; i++)
  {
    auto InputVideoPacket = InputFormatCtx.ReadFrame();

    InputVideoDecoder.SendPacket(InputVideoPacket);
    InputVideoDecoder.ReceiveFrame(DecodedFrame);

    DecodedFrame.Ptr->pts = i;

    DstFrame.CopyProps(DecodedFrame);

    DstFrame.Ptr->format = CodecEncoder.Ptr->pix_fmt;
    DstFrame.Ptr->width = DecodedFrame.Ptr->width;
    DstFrame.Ptr->height = DecodedFrame.Ptr->height;

    DstFrame.GetBuffer();

    sws_scale(SwsCtx, DecodedFrame.Ptr->data, DecodedFrame.Ptr->linesize, 0, Conf.Height, DstFrame.Ptr->data,
              DstFrame.Ptr->linesize);

    CodecEncoder.SendFrame(DstFrame);

    DecodedFrame.UnRef();

    int ret = 0;
    av::StackPacket EncodedPacket;
    EncodedPacket.RawPacket.stream_index = OutputVideoStream->index;
    av_packet_make_refcounted(&EncodedPacket.RawPacket);

    while (ret >= 0)
    {
      ret = CodecEncoder.ReceivePacket(EncodedPacket);
      EncodedPacket.RawPacket.stream_index = OutputVideoStream->index;

      EncodedPacket.RawPacket.pts = av_rescale_q(EncodedPacket.RawPacket.pts, TimebaseRational,
                                                 OutputVideoStream->time_base);
      EncodedPacket.RawPacket.dts = av_rescale_q(EncodedPacket.RawPacket.dts, TimebaseRational,
                                                 OutputVideoStream->time_base);
      EncodedPacket.RawPacket.pos = -1;

      if (ret == 0)
      {
        av::Error WriteErr = av_interleaved_write_frame(OutputFormatCtx.Ptr, &EncodedPacket.RawPacket);
        if (WriteErr)
        {
          printf("Failed to write frame! %d %s \n", WriteErr.Code, WriteErr.Msg.c_str());
          exit(1);
        }
      }
    }

    DstFrame.UnRef();
  }

  av_write_trailer(OutputFormatCtx.Ptr);
  avio_close(OutputFormatCtx.Ptr->pb);
//  avformat_close_input(&OutputFormatCtx);

  return 0;
}
