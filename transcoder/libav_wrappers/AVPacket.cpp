extern "C"
{
#include <libavcodec/avcodec.h>
}

#include "AVPacket.h"

MAVPacket::MAVPacket() : RawPacket(av_packet_alloc()) {}

MAVPacket::~MAVPacket() { av_packet_free(&RawPacket); }
