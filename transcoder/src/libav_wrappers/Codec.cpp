#include "Codec.h"

extern "C"
{
#include "libavformat/avformat.h"
}

av::Codec::Codec(AVCodec* Ptr) : Ptr(Ptr) {}
av::Codec::Codec(AVCodecID CodecId, CodecType Type)
    : Ptr(Type == CodecType::ENCODER ? avcodec_find_encoder(CodecId) : avcodec_find_decoder(CodecId))
{
}

av::Codec::Codec(av::Codec&& P) noexcept : Ptr(P.Ptr) { P.Ptr = nullptr; }

av::Result<av::Codec> av::Codec::FindByID(AVCodecID id)
{
  auto FoundCodecPtr = avcodec_find_decoder(id);

  if (!FoundCodecPtr) return av::Error("Failed to find codec from id");

  return Codec(FoundCodecPtr);
}

bool av::operator==(AVMediaType lType, av::MEDIA_TYPE rType) { return static_cast<MEDIA_TYPE>(lType) == rType; }

// av::CodecContext::CodecContext(av::Codec codec) {}
