#include "FormatContext.h"

av::FormatContext::FormatContext(bool ShouldAlloc)
{
  if (ShouldAlloc) Ptr = avformat_alloc_context();
}

av::FormatContext::~FormatContext()
{
  if (Ptr) avformat_free_context(Ptr);
}

av::StackPacket av::FormatContext::ReadFrame() const
{
  av::StackPacket Packet;
  av_read_frame(Ptr, &Packet.RawPacket);

  return Packet;
}
