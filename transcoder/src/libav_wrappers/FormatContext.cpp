
#include "FormatContext.h"

AV::FormatContext::FormatContext(bool ShouldAlloc)
{
  if (ShouldAlloc) RawContext = avformat_alloc_context();
}
AV::FormatContext::~FormatContext()
{
  if (RawContext) avformat_free_context(RawContext);
}

AVFormatContext *&AV::FormatContext::Data() { return RawContext; }
