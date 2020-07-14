#pragma once

#include "Codec.h"
#include "Frame.h"

extern "C"
{
#include <libavutil/opt.h>
}

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
  explicit CodecContext(AVCodecParameters* Params, av::CodecType CodecType);

  CodecContext(const CodecContext& P);
  CodecContext(CodecContext&& P) noexcept;
  CodecContext& operator=(const CodecContext& Other) = delete;

  AVCodecContext* operator->() { return Ptr; }
  AVCodecContext operator*() { return *Ptr; }

  [[nodiscard]] av::Result<bool> Open() const;

  int SendPacket(av::StackPacket& Packet) const;

  int SendFrame(av::Frame& Frame) const;

  int ReceiveFrame(av::Frame& Frame) const;

  void SetPrivData(const char* Key, int Value) const;

  void SetPrivData(const char* Key, const char* Value) const;

  [[nodiscard]] av::Result<av::StackPacket> ReceivePacket() const;
  int ReceivePacket(av::StackPacket& Packet) const;

  ~CodecContext();
};

} // namespace av
