#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
}

namespace AV

{
class Packet
{
public:
  AVPacket* Ptr = nullptr;
  Packet();
  ~Packet();

  Packet(const Packet& P);
  Packet(Packet&& P) noexcept;

  AVPacket* operator*() const;
  void Clear() const;
  void Destroy();
};

class StackPacket
{
public:
  AVPacket RawPacket{};

  StackPacket();
  ~StackPacket();
  StackPacket(const StackPacket& P);
  StackPacket(StackPacket&& P) noexcept;
  [[nodiscard]] inline bool IsEmpty() const { return RawPacket.buf == nullptr; };

  [[nodiscard]] constexpr const AVPacket& Data() const { return RawPacket; }
  void Clear();
};

} // namespace AV
