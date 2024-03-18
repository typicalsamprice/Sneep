#pragma once
#include <cstdlib>
#include <experimental/source_location>
#include <iostream>

namespace Sneep {
namespace debug {

void dassert(const bool cond,
             const std::experimental::source_location loc =
                 std::experimental::source_location::current());

#if !defined(NDEBUG)
template <typename T>
void print(const T var, const std::experimental::source_location loc =
                            std::experimental::source_location::current()) {
  std::clog << "[" << loc.file_name() << ":" << loc.line() << "] in ("
            << loc.function_name() << "): " << var << std::endl;
}

template <typename T>
[[noreturn]] void error(const T var,
                        const std::experimental::source_location loc =
                            std::experimental::source_location::current()) {
  std::cerr << "[" << loc.file_name() << ":" << loc.line() << "] in ("
            << loc.function_name() << "): " << var << std::endl;
  throw var;
}
#else
template <typename T>
void print(const T var, const std::experimental::source_location loc =
                            std::experimental::source_location::current()) {}
template <typename T>
void error(const T var, const std::experimental::source_location loc =
                            std::experimental::source_location::current()) {}
#endif

} // namespace debug
} // namespace Sneep
