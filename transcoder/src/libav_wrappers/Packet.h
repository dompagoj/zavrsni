#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
}

namespace AV
{
class Packet
{
public:
  AVPacket *RawPacket;
  Packet();
  ~Packet();

  AVPacket *operator*();
  void Unref();

  static AVPacket &GetInstance()
  {
    static AVPacket p;

    return p;
  }
};
} // namespace AV
