#pragma once
struct AVFormatContext;

#include "Packet.h"

namespace AV
{

class FormatContext
{

public:
  AVFormatContext* Ptr{nullptr};

  explicit FormatContext(bool ShouldAlloc = true);
  ~FormatContext();

  constexpr AVFormatContext*& Data() { return Ptr; };

  int ReadFrame(AV::StackPacket& Packet) const;
  [[nodiscard]] StackPacket ReadFrame() const;
};

} // namespace AV
