#pragma once

extern "C"
{
#include <libavformat/avformat.h>
}

namespace AV
{

class FormatContext
{

public:
  AVFormatContext *RawContext{nullptr};

  explicit FormatContext(bool ShouldAlloc = true);
  ~FormatContext();

  AVFormatContext *&Data();
};

} // namespace AV
