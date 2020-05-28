extern "C"
{
#include "libavutil/dict.h"
}

#include "Dictionary.h"
#include <cassert>

av::Dictionary::Dictionary() : Ptr(nullptr) {}

av::Dictionary::~Dictionary() { av_dict_free(&Ptr); }

av::Dictionary::Dictionary(const av::Dictionary& P) { av_dict_copy(&Ptr, P.Ptr, 0); }

av::Dictionary::Dictionary(av::Dictionary&& P) noexcept
{
  assert(P.Ptr != nullptr);
  Ptr = P.Ptr;
  P.Ptr = nullptr;
}

std::optional<std::string_view> av::Dictionary::Get(std::string_view Key) const
{
  auto entry = av_dict_get(Ptr, Key.data(), nullptr, 0);

  if (!entry) return {};

  return entry->value;
}

std::optional<char*> av::Dictionary::Get(const char* Key) const
{
  auto entry = av_dict_get(Ptr, Key, nullptr, 0);

  if (!entry) return {};

  return entry->value;
}

void av::Dictionary::Set(std::string_view Key, std::string_view Value)
{
  av_dict_set(&Ptr, Key.data(), Value.data(), 0);
}

void av::Dictionary::Set(const char* Key, const char* Value) { av_dict_set(&Ptr, Key, Value, 0); }
void av::Dictionary::Set(const char* Key, const int Value) { av_dict_set_int(&Ptr, Key, Value, 0); }
