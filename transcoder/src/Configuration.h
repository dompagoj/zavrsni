#pragma once

#include <string>

class Configuration
{
public:
  int Width;
  int Height;
  int Framerate;
  std::string DeviceName;
  std::string ServerIp;
  int ServerPort;

  static Configuration GetDefault()
  {
    static Configuration Conf = Configuration{
        .Width = 800,
        .Height = 600,
        .Framerate = 15,
        .DeviceName = "Streaming device",
        .ServerIp = "tcp://localhost",
        .ServerPort = 13000,
    };
    return Conf;
  }
};

