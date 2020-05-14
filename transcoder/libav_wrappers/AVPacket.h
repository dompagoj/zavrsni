#pragma once

class MAVPacket
{
public:
  struct AVPacket *RawPacket;
  MAVPacket();
  ~MAVPacket();
};
