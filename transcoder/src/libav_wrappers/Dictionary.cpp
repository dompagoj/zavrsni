extern "C"
{
#include "libavutil/dict.h"
}

#include "Dictionary.h"

AV::Dictionary::Dictionary() : Ptr(nullptr) {}

AV::Dictionary::~Dictionary() { av_dict_free(&Ptr); }

std::optional<std::string_view> AV::Dictionary::Get(std::string_view Key) const
{
  auto entry = av_dict_get(Ptr, Key.data(), nullptr, 0);

  if (!entry) return {};

  return entry->value;
}

std::optional<char*> AV::Dictionary::Get(const char* Key) const
{
  auto entry = av_dict_get(Ptr, Key, nullptr, 0);

  if (!entry) return {};

  return entry->value;
}

void AV::Dictionary::Set(std::string_view Key, std::string_view Value)
{
  av_dict_set(&Ptr, Key.data(), Value.data(), 0);
}

void AV::Dictionary::Set(const char* Key, const char* Value) { av_dict_set(&Ptr, Key, Value, 0); }

AVDictionary*& AV::Dictionary::operator*() { return Ptr; }

AVDictionary*& AV::Dictionary::Data() { return Ptr; }
