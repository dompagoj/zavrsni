#pragma once
extern "C"
{
#include <libavformat/avformat.h>
}

#include "Packet.h"
namespace av
{

class FormatContext
{

public:
  AVFormatContext* Ptr{nullptr};

  explicit FormatContext(bool ShouldAlloc = true);
  ~FormatContext();

  constexpr AVFormatContext*& Data() { return Ptr; };

  [[nodiscard]] inline int ReadFrame(av::StackPacket& Packet) const { return av_read_frame(Ptr, &Packet.RawPacket); }
  [[nodiscard]] StackPacket ReadFrame() const;
};

} // namespace AV
