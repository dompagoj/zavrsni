#include "Codec.h"

extern "C"
{
#include "libavformat/avformat.h"
}

AV::Codec::Codec(AVCodec* Ptr) : Ptr(Ptr) {}

AV::Codec::Codec(AV::Codec&& P) noexcept : Ptr(P.Ptr) { P.Ptr = nullptr; }

AV::Result<AV::Codec> AV::Codec::FindFromAVStreams(AV::FormatContext& Context, CodecType Type)
{
  AV::Codec Codec{};

  for (int i = 0; i < Context.Ptr->nb_streams; i++)
  {
    auto Stream = Context.Ptr->streams[i];
    auto StreamCodec = avcodec_find_decoder(Stream->codecpar->codec_id);
    if (StreamCodec->type == Type)
    {
      Codec.StreamIndex = i;
      Codec.Ptr = StreamCodec;
    }
  }

  if (!Codec.Ptr) return AV::Error("Couldn't't find Stream Codec of type AVMEDIA_TYPE_VIDEO");

  return Codec;
}

AV::Result<AV::Codec> AV::Codec::FindByID(AVCodecID id)
{
  auto FoundCodecPtr = avcodec_find_decoder(id);

  if (!FoundCodecPtr) return AV::Error("Failed to find codec from id");

  return Codec(FoundCodecPtr);
}

bool AV::operator==(AVMediaType lType, AV::CodecType rType) { return static_cast<CodecType>(lType) == rType; }
