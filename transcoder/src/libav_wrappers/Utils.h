#pragma once

#include "../constants.hpp"
#include "FormatContext.h"
#include "Result.h"
#include <vector>

struct AVDeviceInfo;
struct AVInputFormat;

namespace AV::Utils
{

AV::Result<std::vector<AVDeviceInfo*>> FindAllInputDevices(AVInputFormat* InputFormat);
AV::Result<AVInputFormat*> FindInputFormat(const char* DeviceDriver = Constants::GET_VIDEO_DRIVER());
void DumpContext(AV::FormatContext& Context);
void RegisterAllDevices();

} // namespace AV::Utils