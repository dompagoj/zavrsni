#pragma once

#include "Configuration.h"
#include <fstream>
#include <filesystem>

namespace Utils
{
Configuration LoadConfiguration()
{
  auto Path = "../cfg.json";
  if (!std::filesystem::exists(Path)) return Configuration::GetDefault();

  std::ifstream FileStream(Path);
  json J;
  FileStream >> J;

  Configuration Conf;
  Conf.DeviceName = J["deviceName"].get<std::string>();
  Conf.Width = J["width"].get<int>();
  Conf.Height = J["height"].get<int>();
  Conf.Framerate = J["framerate"].get<int>();

  return Conf;
}

}