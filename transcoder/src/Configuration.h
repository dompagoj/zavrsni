#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Configuration
{
public:
  int Width;
  int Height;
  int Framerate;
  std::string DeviceName;

  static const Configuration Test;
  static Configuration GetDefault()
  {
    static Configuration Conf = Configuration{.Width = 800, .Height = 600, .Framerate = 15, .DeviceName = "Streaming device"};
    return Conf;
  }
};

