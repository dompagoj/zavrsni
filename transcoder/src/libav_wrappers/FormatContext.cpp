
#include "FormatContext.h"

AV::FormatContext::FormatContext() : RawContext(avformat_alloc_context()) {}
AV::FormatContext::FormatContext(bool ShouldAlloc) : RawContext(nullptr) {}
AV::FormatContext::~FormatContext()
{
  if (RawContext) avformat_free_context(RawContext);
}
AVFormatContext *AV::FormatContext::Data() const { return RawContext; }
AVFormatContext **AV::FormatContext::DataPtr() { return &RawContext; }

AVFormatContext *AV::FormatContext::operator*() const { return RawContext; }
