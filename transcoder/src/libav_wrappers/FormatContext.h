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
  AVFormatContext *RawContext;
  FormatContext();
  explicit FormatContext(bool ShouldAlloc);
  ~FormatContext();

  [[nodiscard]] AVFormatContext *Data() const;
  [[nodiscard]] AVFormatContext **DataPtr();

  AVFormatContext* operator*() const;
};

} // namespace AV
