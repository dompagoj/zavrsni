#pragma once

#include <concepts>
#include <optional>
#include <string>

struct AVDictionary;

namespace av
{

class Dictionary
{

public:
  struct Opts
  {
    static constexpr auto FRAMERATE = "framerate";
    static constexpr auto VIDEO_SIZE = "video_size";
  };
  AVDictionary* Ptr{nullptr};

  Dictionary();
  ~Dictionary();

  Dictionary(const Dictionary& P);
  Dictionary(Dictionary&& P) noexcept;
  Dictionary& operator=(const Dictionary& Other) = delete;

  [[nodiscard]] std::optional<std::string_view> Get(std::string_view Key) const;
  std::optional<char*> Get(const char* Key) const;

  void Set(std::string_view Key, std::string_view Value);
  void Set(const char* Key, const char* Value);

  [[nodiscard]] inline AVDictionary*& Data() { return Ptr; };
  inline AVDictionary*& operator*() { return Ptr; }
};

} // namespace AV
