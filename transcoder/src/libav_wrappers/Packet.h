#pragma once

namespace AV
{
class Packet
{
public:
  struct AVPacket RawPacket;
  Packet();
  ~Packet();

  struct AVPacket *operator*();
  void Unref();

  static AVPacket &GetInstance()
  {
    static AVPacket p;

    return p;
  }
};
} // namespace AV
