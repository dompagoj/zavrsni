

#include "Packet.h"

// Packet impl
AV::Packet::Packet() : Ptr(av_packet_alloc()) {}

AV::Packet::~Packet() { av_packet_free(&Ptr); }

struct AVPacket* AV::Packet::operator*() const { return Ptr; }

void AV::Packet::Clear() const { av_packet_unref(Ptr); }

void AV::Packet::Destroy() { av_packet_free(&Ptr); }

AV::Packet::Packet(const AV::Packet& P)
{
  Ptr = av_packet_alloc();
  av_packet_ref(Ptr, P.Ptr);
}

AV::Packet::Packet(AV::Packet&& P) noexcept { av_packet_move_ref(Ptr, P.Ptr); }

// StackPacket impl
AV::StackPacket::StackPacket() { av_init_packet(&RawPacket); }
AV::StackPacket::~StackPacket() { av_packet_unref(&RawPacket); }
AV::StackPacket::StackPacket(const AV::StackPacket& P)
{
  av_init_packet(&RawPacket);
  av_packet_ref(&RawPacket, &P.RawPacket);
}
AV::StackPacket::StackPacket(AV::StackPacket&& P) noexcept { av_packet_move_ref(&RawPacket, &P.RawPacket); }

void AV::StackPacket::Clear() { av_packet_unref(&RawPacket); }
