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

  CodecContext() = default;
  explicit CodecContext(const av::Codec& Codec);
  explicit CodecContext(AVCodecParameters* Params);

  CodecContext(const CodecContext& P);
  CodecContext(CodecContext&& P) noexcept;
  CodecContext& operator=(const CodecContext& Other) = delete;

  [[nodiscard]] av::Result<bool> Open() const;

  int SendPacket(av::StackPacket& Packet) const;

  [[nodiscard]] av::Result<av::StackPacket> ReceivePacket() const;
  int ReceivePacket(av::StackPacket& Packet) const;

  ~CodecContext();
};

} // namespace av
