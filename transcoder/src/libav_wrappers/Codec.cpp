#include "Codec.h"

extern "C"
{
#include "libavformat/avformat.h"
}

av::Codec::Codec(AVCodec* Ptr) : Ptr(Ptr) {}

av::Codec::Codec(av::Codec&& P) noexcept : Ptr(P.Ptr) { P.Ptr = nullptr; }

av::Result<av::Codec> av::Codec::FindFromAVStreams(av::FormatContext& Context, CodecType Type)
{
  av::Codec Codec{};

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

  if (!Codec.Ptr) return av::Error("Couldn't't find Stream Codec of type AVMEDIA_TYPE_VIDEO");

  return Codec;
}

av::Result<av::Codec> av::Codec::FindByID(AVCodecID id)
{
  auto FoundCodecPtr = avcodec_find_decoder(id);

  if (!FoundCodecPtr) return av::Error("Failed to find codec from id");

  return Codec(FoundCodecPtr);
}

bool av::operator==(AVMediaType lType, av::CodecType rType) { return static_cast<CodecType>(lType) == rType; }

av::CodecContext::CodecContext(av::Codec codec) {}
