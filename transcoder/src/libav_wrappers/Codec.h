#pragma once

#include "FormatContext.h"
#include "Result.h"

struct AVCodec;

namespace AV
{

enum class CodecType
{
  VIDEO = AVMEDIA_TYPE_VIDEO,
  AUDIO = AVMEDIA_TYPE_AUDIO,
  UNKNOWN = AVMEDIA_TYPE_UNKNOWN, ///< Usually treated as AVMEDIA_TYPE_DATA
  DATA = AVMEDIA_TYPE_DATA,       ///< Opaque data information usually continuous
  SUBTITLE = AVMEDIA_TYPE_SUBTITLE,
  ATTACHMENT = AVMEDIA_TYPE_ATTACHMENT, ///< Opaque data information usually sparse
};

class Codec
{
public:
  AVCodec* Ptr = nullptr;
  int StreamIndex{};

  Codec() = default;
  explicit Codec(AVCodec* Ptr);
  Codec(const Codec& P) = default;
  Codec(Codec&& P) noexcept;

  static AV::Result<Codec> FindFromAVStreams(FormatContext& Context, CodecType Type = CodecType::VIDEO);
  static AV::Result<Codec> FindByID(AVCodecID id);
};

bool operator==(AVMediaType lType, CodecType rType);
} // namespace AV
