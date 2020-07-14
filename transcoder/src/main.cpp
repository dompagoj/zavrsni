#include "libav_wrappers/Codec.h"
#include "libav_wrappers/CodecContext.h"
#include "libav_wrappers/Utils.h"
#include "Configuration.h"
#include "Utils.hpp"

extern "C"
{
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

int main()
{
  av_log_set_level(AV_LOG_DEBUG);
  av::Utils::RegisterAllDevices();

  auto Conf = utils::LoadConfiguration();

  AVInputFormat* InputFormat = av::Utils::FindInputFormat(Constants::GET_VIDEO_DRIVER())
      .Expect("Failed to find input format");

  auto DeviceName = av::Utils::FindInputDevice(InputFormat).Expect("Failed to find all input devices");

  av::Dictionary Opts;
  Opts.Set("framerate", Conf.Framerate);
  Opts.Set("video_size", av::Utils::GetResolutionString(Conf.Width, Conf.Height));

  av::FormatContext InputFormatCtx;
  InputFormatCtx.OpenInput(DeviceName, InputFormat, Opts);

  InputFormatCtx.Dump();

  auto InputVideoStream = av::Utils::FindStreamFromFormatCtx(InputFormatCtx, av::MEDIA_TYPE::VIDEO);
  if (!InputVideoStream) return utils::Terminate("Couldn't find input video stream");

  InputVideoStream->start_time = 0;

  auto FramerateRational = InputVideoStream->r_frame_rate;
  auto TimebaseRational = av_inv_q(FramerateRational);
  Conf.Width = InputVideoStream->codecpar->width;
  Conf.Height = InputVideoStream->codecpar->height;

  av::CodecContext InputVideoDecoder(InputVideoStream->codecpar, av::CodecType::DECODER);
  InputVideoDecoder->rc_buffer_size = 0;
  InputVideoDecoder->delay = 0;

  InputVideoDecoder.Open().Expect("Failed to open InputVideoDecoder");

  av::CodecContext CodecEncoder(av::Codec(AV_CODEC_ID_VP9, av::CodecType::ENCODER));

  CodecEncoder->width = InputVideoDecoder.Ptr->width;
  CodecEncoder->height = InputVideoDecoder.Ptr->height;
  CodecEncoder->time_base = TimebaseRational;
  CodecEncoder->pix_fmt = AV_PIX_FMT_YUV420P;
  CodecEncoder->gop_size = 15;
  CodecEncoder->max_b_frames = 0;
  CodecEncoder->thread_count = 1;
  CodecEncoder->bit_rate = 0;
  CodecEncoder->qmin = 4;
  CodecEncoder->qmax = 48;
  CodecEncoder->delay = 0;

  CodecEncoder.SetPrivData("quality", "realtime");
  CodecEncoder.SetPrivData("b:v", "5M");
  CodecEncoder.SetPrivData("minrate", "5M");
  CodecEncoder.SetPrivData("maxrate", "5M");
  CodecEncoder.SetPrivData("lag-in-frames", 0);
  CodecEncoder.SetPrivData("frame-parallel", 1);
  CodecEncoder.SetPrivData("tile-columns", 4);
  CodecEncoder.SetPrivData("threads", 1);
  CodecEncoder.SetPrivData("static-thresh", 0);
  CodecEncoder.SetPrivData("max-intra-rate", 300);
  CodecEncoder.SetPrivData("row-mt", 1);

  CodecEncoder.Open().Expect("Failed to open VP9 Encoder");

  av::FormatContext OutputFormatCtx;
  OutputFormatCtx.AllocOutput("WebM").Unwrap();

  std::string ServerUrl = Conf.ServerIp + ":" + std::to_string(Conf.ServerPort);
  OutputFormatCtx.OpenAVIOContext(ServerUrl).Expect("Failed to open AVIO Context");

  OutputFormatCtx->oformat->video_codec = CodecEncoder.Codec->id;
  OutputFormatCtx->flags |= AVFMT_FLAG_NOBUFFER;
  OutputFormatCtx->bit_rate = 0;
  if (OutputFormatCtx.Ptr->oformat->flags & AVFMT_GLOBALHEADER)
    CodecEncoder.Ptr->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

  auto OutputVideoStream = avformat_new_stream(OutputFormatCtx.Ptr, CodecEncoder.Codec);
  avcodec_parameters_from_context(OutputVideoStream->codecpar, CodecEncoder.Ptr);
  OutputVideoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  OutputVideoStream->avg_frame_rate = FramerateRational;

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

    DecodedFrame->pts = i;

    DstFrame.CopyProps(DecodedFrame);

    DstFrame->format = CodecEncoder.Ptr->pix_fmt;
    DstFrame->width = DecodedFrame.Ptr->width;
    DstFrame->height = DecodedFrame.Ptr->height;
    DstFrame->pict_type = AV_PICTURE_TYPE_NONE;

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

      EncodedPacket.RawPacket.pts = av_rescale_q(EncodedPacket.RawPacket.pts, CodecEncoder->time_base,
                                                 OutputVideoStream->time_base);
      EncodedPacket.RawPacket.dts = av_rescale_q(EncodedPacket.RawPacket.dts, CodecEncoder->time_base,
                                                 OutputVideoStream->time_base);
      EncodedPacket.RawPacket.pos = -1;

      if (ret == 0)
      {
        OutputFormatCtx.WriteFrame(EncodedPacket).Unwrap();
      }
    }

    DstFrame.UnRef();
  }

}
