extern "C"
{
#include <libavformat/avformat.h>
}
#include "FormatContext.h"

AV::FormatContext::FormatContext(bool ShouldAlloc)
{
  if (ShouldAlloc) Ptr = avformat_alloc_context();
}

AV::FormatContext::~FormatContext()
{
  if (Ptr) avformat_free_context(Ptr);
}

int AV::FormatContext::ReadFrame(AV::StackPacket& Packet) const { return av_read_frame(Ptr, &Packet.RawPacket); }
AV::StackPacket AV::FormatContext::ReadFrame() const
{
  AV::StackPacket Packet;
  av_read_frame(Ptr, &Packet.RawPacket);

  return Packet;
}
