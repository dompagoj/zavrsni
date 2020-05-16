#include "Packet.h"

AV::Packet::Packet() : RawPacket() {}

AV::Packet::~Packet() { av_packet_unref(&GetInstance()); }

struct AVPacket *AV::Packet::operator*() { return RawPacket; }
void AV::Packet::Unref() { av_packet_unref(RawPacket); }
