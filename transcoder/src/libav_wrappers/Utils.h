#pragma once

#include "../constants.hpp"
#include "FormatContext.h"
#include "Result.hpp"
#include <string>

struct AVDeviceInfo;
struct AVInputFormat;

namespace av::Utils
{

av::Result<std::string> FindInputDevice(AVInputFormat* InputFormat, int Index = -1);
av::Result<AVInputFormat*> FindInputFormat(const char* DeviceDriver = Constants::GET_VIDEO_DRIVER());
void DumpContext(av::FormatContext& Context);
void RegisterAllDevices();

} // namespace AV::Utils