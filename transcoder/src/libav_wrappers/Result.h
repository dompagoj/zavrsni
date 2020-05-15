#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
#pragma once

#include <optional>
#include <string>

namespace AV
{

template <typename T> class Result
{
  struct Error
  {
    int Code{};
    std::string Msg;

    Error(int Code) : Code{Code} {}
  };

  std::optional<T> t;
  std::optional<Error> error;

public:
  operator bool() { return error; }

  Result(const T &t) : t{t} {}
  Result(const T &&t) : t{std::move(t)} {}

  Result(int ErrCode) : error{ErrCode} {}

  bool HasError() { return error; }

  bool HasData();
};

} // namespace AV

#pragma clang diagnostic pop