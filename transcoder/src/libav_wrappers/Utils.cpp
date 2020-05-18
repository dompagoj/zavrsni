#include "Utils.h"
#include <cassert>
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

  std::string DeviceName;
  DeviceName = List->devices[DefaultIndex]->device_name;

  avdevice_free_list_devices(&List);

  return DeviceName;
}
av::Result<AVInputFormat*> av::Utils::FindInputFormat(const char* DeviceDriver)
{
  return av_find_input_format(DeviceDriver);
}

void av::Utils::DumpContext(av::FormatContext& Context) { av_dump_format(Context.Ptr, 0, nullptr, 0); }
void av::Utils::RegisterAllDevices() { avdevice_register_all(); }
