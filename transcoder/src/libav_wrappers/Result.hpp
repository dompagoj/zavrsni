#pragma once

#include <optional>
#include <string>

extern "C"
{
#include "libavutil/error.h"
}

namespace av
{

class Error
{
public:
  int Code{};
  std::string Msg;

  Error(int Code) : Code{Code}
  {
    char Buffer[AV_ERROR_MAX_STRING_SIZE];
    av_make_error_string(Buffer, AV_ERROR_MAX_STRING_SIZE, Code);
    Msg = std::string(Buffer);
  }

  Error(const std::string_view& Msg) : Code(-1), Msg(Msg) {}
};

template<typename T>
class Result
{
  std::optional<T> t;
  std::optional<Error> error;

public:
  inline operator bool() { return t.has_value(); }
  inline T operator*() { return *t; }

  Result(const T& t) : t{t} {}
  Result(const T&& t) : t{std::move(t)} {}

  Result(Error ErrCode) : error{std::optional{ErrCode}} {}

  [[nodiscard]] bool HasError() const { return !!error; }

  [[nodiscard]] bool HasData() const { return !!t; }

  T Expect(std::string_view const& Message) const
  {
    if (error)
    {
      printf("%s \n Actual error: %d %s \n", Message.data(), error->Code, error->Msg.c_str());
      exit(1);
    }

    return *t;
  }

  T Unwrap() const
  {
    if (error)
    {
      printf("Error while unwrapping: %d %s \n", error->Code, error->Msg.c_str());
      exit(1);
    }

    return *t;
  }

  [[nodiscard]] T UnwrapOr(T const& Default) const
  {
    if (error) return Default;

    return *t;
  }

  [[nodiscard]] inline Error Err() const { return *error; }
  [[nodiscard]] inline T Value() const { return *t; }
};

} // namespace AV
