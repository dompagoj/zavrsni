#include "Utils.h"
#include <cstdint>

extern "C"
{
#include <libavdevice/avdevice.h>
}

AV::Result<std::vector<AVDeviceInfo *>> AV::Utils::FindAllInputDevices(AVInputFormat *InputFormat)
{
  AVDeviceInfoList *List = nullptr;

  int err2 = avdevice_list_input_sources(InputFormat, nullptr, nullptr, &List);

  if (err2 < 0) return AV::Error(err2);

  std::vector<AVDeviceInfo *> Devices;

  for (auto i = List->nb_devices - 1; i > 0; i--)
  {
    auto device = List->devices[i];
    auto found = std::find_if(Devices.begin(), Devices.end(), [device](const AVDeviceInfo *vecDevice) {
      return strcmp(vecDevice->device_description, device->device_description) == 0;
    });

    if (found != Devices.end()) continue;

    Devices.push_back(device);
  }

  return Devices;
}
