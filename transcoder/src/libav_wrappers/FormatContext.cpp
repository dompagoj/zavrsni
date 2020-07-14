#include "FormatContext.h"

av::FormatContext::FormatContext() { Ptr = avformat_alloc_context(); }

av::FormatContext::~FormatContext()
{
  if (Ptr) avformat_close_input(&Ptr);
}

av::StackPacket av::FormatContext::ReadFrame() const
{
  av::StackPacket Packet;
  av_read_frame(Ptr, &Packet.RawPacket);

  return Packet;
}

void av::FormatContext::Dump() const { av_dump_format(Ptr, 0, nullptr, 0); }

void av::FormatContext::OpenInput(std::string_view DeviceName, AVInputFormat* InputFormat, av::Dictionary& Opts)
{
  avformat_open_input(&Ptr, DeviceName.data(), InputFormat, &*Opts);
}

av::Result<int> av::FormatContext::WriteHeader(av::Dictionary& Opts)
{
  int write_header_err = avformat_write_header(Ptr, &Opts.Ptr);
  if (write_header_err < 0) return av::Error(write_header_err);

  return 0;
}

av::Result<int> av::FormatContext::AllocOutput(const char* FormatName)
{
  int err = avformat_alloc_output_context2(&Ptr, nullptr, FormatName, nullptr);
  if (err < 0) return av::Error(err);

  return 0;
}
av::Result<int> av::FormatContext::OpenAVIOContext(const std::string& Url)
{
  int err = avio_open2(&Ptr->pb, Url.c_str(), AVIO_FLAG_WRITE, nullptr, nullptr);

  if (err < 0) return av::Error(err);

  return 0;
}
av::Result<int> av::FormatContext::WriteFrame(av::StackPacket& Packet)
{
  int err = av_interleaved_write_frame(Ptr, &Packet.RawPacket);
  if (err < 0)
  {
    return av::Error(err);
  }

  return 0;
}
