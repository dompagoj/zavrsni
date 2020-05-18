#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma once

#include "string.h"
#include <concepts>
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

namespace AV
{

struct Error
{
  int Code{};
  std::string Msg;

  Error(int Code) : Code{Code}
  {

#if defined(__clang__)
    Msg = av_err2str(Code);
#else
    char Buffer[AV_ERROR_MAX_STRING_SIZE];
    av_make_error_string(Buffer, AV_ERROR_MAX_STRING_SIZE, Code);
    Msg = std::string(Buffer);
#endif
  }
  Error(const std::string_view& Msg) : Msg(Msg), Code(-1) {}
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
      printf("%s \n %s at Line: %d \n Actual error: %s \n", Message.data(), M_FILENAME, __LINE__,
             error.value().Msg.c_str());
      exit(1);
    }

    return *t;
  }

  T Unwrap() const
  {
    if (error)
    {
      printf("%s \n %s at Line: %d \n", error.value().Msg.c_str(), M_FILENAME, __LINE__);
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

#pragma clang diagnostic pop