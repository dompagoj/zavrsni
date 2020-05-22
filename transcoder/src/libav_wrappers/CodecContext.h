#pragma once

#include "Codec.h"
struct AVCodecContext;

namespace av
{

class CodecContext
{
public:
  AVCodecContext* Ptr{nullptr};
  AVCodec* Codec{nullptr};

  explicit CodecContext(const av::Codec& Codec);

  CodecContext(const CodecContext& P);
  CodecContext(CodecContext&& P) noexcept;
  CodecContext& operator=(const CodecContext& Other) = delete;

  void Open() const;

  av::Result<int> SendPacket(av::StackPacket& Packet) const;

  av::Result<av::StackPacket> ReceivePacket();
  int ReceivePacket(av::StackPacket& Packet) const;

  ~CodecContext();
};

} // namespace av
