#pragma once

#include "../constants.hpp"
#include "Codec.h"
#include "FormatContext.h"
#include "Result.hpp"
#include <string>

struct AVInputFormat;

namespace av::Utils
{

av::Result<std::string> FindInputDevice(AVInputFormat* InputFormat, int Index = -1);
av::Result<AVInputFormat*> FindInputFormat(const char* DeviceDriver = Constants::GET_VIDEO_DRIVER());
void RegisterAllDevices();
AVCodecParameters* FindContextParamsFromFormatStreams(const av::FormatContext& FormatCtx, av::MEDIA_TYPE Type);

} // namespace AV::Utils