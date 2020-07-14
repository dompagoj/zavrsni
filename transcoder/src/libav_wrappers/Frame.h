#pragma once

extern "C"
{
#include <libavutil/frame.h>
}

namespace av
{

class Frame
{
public:
  AVFrame* Ptr;

  Frame();
  ~Frame();

  int GetBuffer(int align = 0);

  int CopyProps(const av::Frame& Other);

  void UnRef();

  AVFrame* operator->() { return Ptr; };
  AVFrame operator*() { return *Ptr; };

};

}

