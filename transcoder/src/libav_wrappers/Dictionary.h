#pragma once

#include <optional>
#include <string>

struct AVDictionary;

namespace AV
{

class Dictionary
{

public:
  AVDictionary *RawDict;
  Dictionary();
  ~Dictionary();

  [[nodiscard]] AVDictionary *Data() const;
  AVDictionary **DataPtr();

  [[nodiscard]] std::optional<std::string_view> Get(std::string_view Key) const;
  std::optional<char *> Get(const char *Key) const;

  void Set(std::string_view Key, std::string_view Value);
  void Set(const char *Key, const char *Value);

  std::optional<const char *> operator[](const char *key) const;

  AVDictionary *operator*() const;
};

} // namespace AV
