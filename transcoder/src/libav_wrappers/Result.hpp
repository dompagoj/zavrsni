#pragma once

#include <optional>
#include <string>

extern "C"
{
#include "libavutil/error.h"
}

#if defined(__FILE_NAME__)
#define M_FILENAME __FILE_NAME__
#elif defined(__FILE__)
#define M_FILENAME __FILE__
#else
#define M_FILENAME "Unknown source file"
#endif

namespace av
{

struct Error
{
  int Code{};
  std::string Msg;

  Error(int Code) : Code{Code}
  {

#if defined(__clang__)
    char Buffer[AV_ERROR_MAX_STRING_SIZE];
    av_make_error_string(Buffer, AV_ERROR_MAX_STRING_SIZE, Code);
    Msg = std::string(Buffer);
#else
    char Buffer[AV_ERROR_MAX_STRING_SIZE];
    av_make_error_string(Buffer, AV_ERROR_MAX_STRING_SIZE, Code);
    Msg = std::string(Buffer);
#endif
  }
  Error(const std::string_view& Msg) : Code(-1), Msg(Msg) {}
};

template <typename T> class Result
{

  std::optional<T> t;
  std::optional<Error> error;

public:
  inline operator bool() { return t.has_value(); }
  inline T operator*() { return *t; }

  Result(const T& t) : t{t} {}
  Result(const T&& t) : t{std::move(t)} {}

  Result(Error ErrCode) : error{ErrCode} {}

  [[nodiscard]] bool HasError() const { return !!error; }

  [[nodiscard]] bool HasData() const { return t; }

  T Expect(std::string_view const& Message) const
  {
    if (error)
    {
      printf("%s \n Actual error: %s \n", Message.data(), error.value().Msg.c_str());
      exit(1);
    }

    return *t;
  }

  T Unwrap() const
  {
    if (error)
    {
      printf("Error while unwrapping:  %s \n", error.value().Msg.c_str());
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
