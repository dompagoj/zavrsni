#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma once

#include <optional>
#include <string>

extern "C"
{
#include "libavutil/error.h"
}

namespace AV
{

struct Error
{
  int Code{};
  std::string Msg;

  Error(int Code) : Code{Code} { Msg = av_err2str(Code); }
};

template <typename T> class Result
{

  std::optional<T> t;
  std::optional<Error> error;

public:
  operator bool() { return error.has_value(); }

  Result(const T &t) : t{t} {}
  Result(const T &&t) : t{std::move(t)} {}

  Result(Error ErrCode) : error{ErrCode} {}

  [[nodiscard]] bool HasError() const { return error.has_value(); }

  [[nodiscard]] bool HasData() const { return t; }

  T Expect(std::string &Message) const
  {
    if (error)
    {
      printf("%s", Message.c_str());
      exit(1);
    }

    return t;
  }

  T Unwrap() { return t; }

  T UnwrapOr(T Default)
  {
    if (error) return Default;

    return t;
  }

  Error Err() { return *error; }
  T Data() { return *t; }
};

} // namespace AV

#pragma clang diagnostic pop