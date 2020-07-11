#pragma once

#include "FormatContext.h"
#include "Result.hpp"

struct AVCodec;

namespace av
{

enum class MEDIA_TYPE
{
  VIDEO = AVMEDIA_TYPE_VIDEO,
  AUDIO = AVMEDIA_TYPE_AUDIO,
  UNKNOWN = AVMEDIA_TYPE_UNKNOWN, ///< Usually treated as AVMEDIA_TYPE_DATA
  DATA = AVMEDIA_TYPE_DATA,       ///< Opaque data information usually continuous
  SUBTITLE = AVMEDIA_TYPE_SUBTITLE,
  ATTACHMENT = AVMEDIA_TYPE_ATTACHMENT, ///< Opaque data information usually sparse
};

enum class CodecType
{
  DECODER,
  ENCODER,
};

class Codec
{
public:
  AVCodec* Ptr{nullptr};
  int StreamIndex{0};

  Codec() = default;
  explicit Codec(AVCodec* Ptr);
  explicit Codec(AVCodecID CodecId, CodecType Type);
  Codec(const Codec& P) = default;
  Codec(Codec&& P) noexcept;

  static av::Result<Codec> FindByID(AVCodecID id, CodecType Type);
};
bool operator==(AVMediaType lType, MEDIA_TYPE rType);

} // namespace AV
