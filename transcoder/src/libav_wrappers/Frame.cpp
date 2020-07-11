#include "Frame.h"

av::Frame::Frame()
{
  Ptr = av_frame_alloc();
}
av::Frame::~Frame()
{
  av_frame_free(&Ptr);
}

int av::Frame::GetBuffer(int align)
{
  return av_frame_get_buffer(Ptr, align);
}

void av::Frame::UnRef()
{
  av_frame_unref(Ptr);
}

int av::Frame::CopyProps(const av::Frame& Other)
{
  return av_frame_copy_props(Ptr, Other.Ptr);
}
