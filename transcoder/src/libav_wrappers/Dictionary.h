#pragma once

#include <concepts>
#include <optional>
#include <string>

struct AVDictionary;

namespace AV
{

class Dictionary
{

public:
  struct Opts
  {
    static constexpr auto FRAMERATE = "framerate";
    static constexpr auto VIDEO_SIZE = "video_size";
  };

  AVDictionary* Ptr;
  Dictionary();
  ~Dictionary();

  [[nodiscard]] AVDictionary*& Data();

  [[nodiscard]] std::optional<std::string_view> Get(std::string_view Key) const;
  std::optional<char*> Get(const char* Key) const;

  void Set(std::string_view Key, std::string_view Value);
  void Set(const char* Key, const char* Value);

  AVDictionary*& operator*();
};

} // namespace AV
