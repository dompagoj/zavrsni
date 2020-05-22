#include "Packet.h"

// Packet impl
// av::Packet::Packet() : Ptr(av_packet_alloc()) {}
//
// av::Packet::~Packet()
//{
//  if (Ptr != nullptr) av_packet_free(&Ptr);
//}
//
// av::Packet::Packet(const av::Packet& P)
//{
//  Ptr = av_packet_alloc();
//  av_packet_ref(Ptr, P.Ptr);
//}
//
// av::Packet::Packet(av::Packet&& P) noexcept { av_packet_move_ref(Ptr, P.Ptr); }
// AVPacket* av::Packet::operator*() const { return Ptr; }
//
// void av::Packet::Clear() const { av_packet_unref(Ptr); }
//
// void av::Packet::Destroy() { av_packet_free(&Ptr); }

// StackPacket impl
av::StackPacket::StackPacket() { av_init_packet(&RawPacket); }
av::StackPacket::~StackPacket() { av_packet_unref(&RawPacket); }
av::StackPacket::StackPacket(av::StackPacket&& P) noexcept { av_packet_move_ref(&RawPacket, &P.RawPacket); }

av::StackPacket::StackPacket(const av::StackPacket& P)
{
  av_init_packet(&RawPacket);
  av_packet_ref(&RawPacket, &P.RawPacket);
}
