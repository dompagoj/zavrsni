#include "Utils.h"
#include "Codec.h"
#include <cstdint>

extern "C"
{
#include <libavdevice/avdevice.h>
}

void PrintDevices(AVDeviceInfoList* Devices, int Selected = 0)
{
  printf("Found devices: \n");
  int NumDevices = Devices->nb_devices;
  for (int i = NumDevices - 1; i >= 0; --i)
  {
    int PrintIndex = NumDevices - i - 1;
    auto Device = Devices->devices[i];
    // device_name is eg /dev/video
    if (i == Selected)
      printf("[%d][*] Device name: %s | Device location: %s \n", PrintIndex, Device->device_description,
             Device->device_name);
    else
      printf("[%d] Device name: %s | Device location: %s \n", PrintIndex, Device->device_description,
             Device->device_name);
  }
}

av::Result<std::string> av::Utils::FindInputDevice(AVInputFormat* InputFormat, int Index)
{
  AVDeviceInfoList* List = nullptr;
  int err2 = avdevice_list_input_sources(InputFormat, nullptr, nullptr, &List);
  if (err2 < 0) return av::Error(err2);

  auto DefaultIndex = List->nb_devices - 1;
  auto ActualIndex = Index != -1 ? DefaultIndex - Index : DefaultIndex;

  PrintDevices(List, ActualIndex);

  std::string DeviceName = List->devices[DefaultIndex]->device_name;
  avdevice_free_list_devices(&List);

  return DeviceName;
}

av::Result<AVInputFormat*> av::Utils::FindInputFormat(const char* DeviceDriver)
{
  return av_find_input_format(DeviceDriver);
}

void av::Utils::RegisterAllDevices() { avdevice_register_all(); }

AVStream* av::Utils::FindStreamFromFormatCtx(const av::FormatContext& FormatCtx, av::MEDIA_TYPE Type)
{
  for (unsigned int i = 0; i < FormatCtx.Ptr->nb_streams; i++)
  {
    auto Stream = FormatCtx.Ptr->streams[i];
    if (Stream->codecpar->codec_type == Type) { return Stream; }
  }

  return nullptr;
}

std::string av::Utils::GetResolutionString(const int WIDTH, const int HEIGHT)
{
  return std::to_string(WIDTH) + "x" + std::to_string(HEIGHT);
}
