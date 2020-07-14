#pragma once

#include "Configuration.h"
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


namespace utils
{
constexpr auto LocalConfPath = "./cfg.json";
constexpr auto SystemConfPath = "/etc/hst/cfg.json";

template<typename T>
T JsonGetOr(const json& J, const std::string& Key, const T& Default)
{
  return J.contains(Key) ? J[Key].get<T>() : Default;
}

Configuration LoadConfiguration()
{
  std::string_view FoundPath;
  auto DefaultConf = Configuration::GetDefault();

  if (std::filesystem::exists(LocalConfPath)) FoundPath = LocalConfPath;
#ifdef linux
  else if (std::filesystem::exists(SystemConfPath)) FoundPath = LocalConfPath;
#endif
  else return DefaultConf;

  std::ifstream FileStream(FoundPath.data());
  json J;
  FileStream >> J;

  return {
      .Width =   JsonGetOr(J, "width", DefaultConf.Width),
      .Height = JsonGetOr(J, "height", DefaultConf.Height),
      .Framerate = JsonGetOr(J, "framerate", DefaultConf.Framerate),
      .DeviceName = JsonGetOr(J, "deviceName", DefaultConf.DeviceName),
      .ServerIp = JsonGetOr(J, "serverIp", DefaultConf.ServerIp),
      .ServerPort = JsonGetOr(J, "serverPort", DefaultConf.ServerPort),
  };
}

int Terminate(const std::string& Msg)
{
  printf("%s \n", Msg.c_str());
  exit(1);

  return 1;
}

static int count = 0;
static int p_count = 0;
static double IFrameSize = 0;
static double PFrameSize = 0;

void TrackSizes(const av::StackPacket& Packet, bool IsIFrame)
{
  if (IsIFrame)
  {
    IFrameSize += Packet.RawPacket.size;
    count++;
  } else
  {
    PFrameSize += Packet.RawPacket.size;
    p_count++;
  }

  if (count == 5)
  {
    printf("IFrame size: %lf \n PFrame size: %lf \n", IFrameSize / count, PFrameSize / p_count);
    exit(1);
  }
}

}