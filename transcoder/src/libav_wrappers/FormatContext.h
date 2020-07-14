#pragma once
extern "C"
{
#include <libavformat/avformat.h>
}

#include "Dictionary.h"
#include "Packet.h"
#include "Result.hpp"
#include <string_view>

namespace av
{

class FormatContext
{
public:
  AVFormatContext* Ptr{nullptr};

  explicit FormatContext();
  ~FormatContext();

  av::Result<int> AllocOutput(const char* FormatName);

  constexpr AVFormatContext*& Data() { return Ptr; };

  AVFormatContext* operator->() { return Ptr; }
  AVFormatContext operator*() { return *Ptr; }

  [[nodiscard]] inline int ReadFrame(av::StackPacket& Packet) const { return av_read_frame(Ptr, &Packet.RawPacket); }
  [[nodiscard]] StackPacket ReadFrame() const;

  void Dump() const;
  inline void Close() { avformat_close_input(&Ptr); }
  void OpenInput(std::string_view Devicename, AVInputFormat* InputFormat, av::Dictionary& Opts);
  av::Result<int> WriteHeader(av::Dictionary& Opts);

  av::Result<int> OpenAVIOContext(const std::string& Url);

  av::Result<int> WriteFrame(av::StackPacket& Packet);
};

} // namespace AV
