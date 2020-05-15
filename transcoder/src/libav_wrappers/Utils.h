#pragma once

#include "Result.h"
#include <vector>

struct AVDeviceInfo;
struct AVInputFormat;

namespace AV::Utils
{

AV::Result<std::vector<AVDeviceInfo *>> FindAllInputDevices(AVInputFormat *InputFormat);

} // namespace AV::Utils