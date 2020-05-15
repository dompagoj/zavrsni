extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "Packet.h"

AV::Packet::Packet() : RawPacket(av_packet_alloc()) {}

AV::Packet::~Packet() { av_packet_free(&RawPacket); }

struct AVPacket *AV::Packet::operator*() { return RawPacket; }
