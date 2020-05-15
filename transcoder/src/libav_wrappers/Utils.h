#pragma once

#include <vector>

struct AVDeviceInfo;
struct AVInputFormat;

namespace AV::Utils
{

std::vector<AVDeviceInfo*> FindAllInputDevices(AVInputFormat *InputFormat);

}