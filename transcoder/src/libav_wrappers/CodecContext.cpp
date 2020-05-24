#include "CodecContext.h"

av::CodecContext::CodecContext(av::Codec const& Codec)
{
  Ptr = avcodec_alloc_context3(Codec.Ptr);
  this->Codec = Codec.Ptr;
}
av::CodecContext::CodecContext(AVCodecParameters* Params)
{
  Codec = avcodec_find_decoder(Params->codec_id);
  Ptr = avcodec_alloc_context3(Codec);
  avcodec_parameters_to_context(Ptr, Params);
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
av::Result<bool> av::CodecContext::Open() const
{
  int err = avcodec_open2(Ptr, nullptr, nullptr);
  if (err < 0) return av::Error(err);

  return true;
}

int av::CodecContext::SendPacket(av::StackPacket& Packet) const { return avcodec_send_packet(Ptr, &Packet.RawPacket); }

av::Result<av::StackPacket> av::CodecContext::ReceivePacket() const
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
