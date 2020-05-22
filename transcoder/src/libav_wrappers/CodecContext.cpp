#include "CodecContext.h"

av::CodecContext::CodecContext(av::Codec const& Codec)
{
  Ptr = avcodec_alloc_context3(Codec.Ptr);
  this->Codec = Codec.Ptr;
}
av::CodecContext::~CodecContext() { avcodec_free_context(&Ptr); }

av::CodecContext::CodecContext(const av::CodecContext& P)
{
  AVCodecParameters* Params{nullptr};
  avcodec_parameters_from_context(Params, P.Ptr);

  AVCodecContext* NewContextPtr{nullptr};
  avcodec_parameters_to_context(NewContextPtr, Params);
  Ptr = NewContextPtr;
  avcodec_parameters_free(&Params);
}

av::CodecContext::CodecContext(av::CodecContext&& P) noexcept
{
  Ptr = P.Ptr;
  P.Ptr = nullptr;
}
void av::CodecContext::Open() const { avcodec_open2(Ptr, Codec, nullptr); }

av::Result<int> av::CodecContext::SendPacket(av::StackPacket& Packet) const
{
  bool IsOpen = avcodec_is_open(Ptr);
  if (!IsOpen)
  {
    printf("Codec is not open!");
    exit(1);
  }

  bool IsDecoder = av_codec_is_decoder(Ptr->codec);
  if (IsDecoder) { printf("Is decoder true! \n"); }

  bool IsEncoder = av_codec_is_encoder(Ptr->codec);
  if (IsEncoder) { printf("Is encoder true! \n"); }

  int err = avcodec_send_packet(Ptr, &Packet.RawPacket);
  if (err < 0) return av::Error(err);

  return 0;
}
av::Result<av::StackPacket> av::CodecContext::ReceivePacket()
{
  av::StackPacket Packet;
  int err = avcodec_receive_packet(Ptr, &Packet.RawPacket);

  if (err < 0) return av::Error(err);

  return Packet;
}
int av::CodecContext::ReceivePacket(av::StackPacket& Packet) const
{
  return avcodec_receive_packet(Ptr, &Packet.RawPacket);
}
