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
