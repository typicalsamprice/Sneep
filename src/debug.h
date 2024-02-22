#pragma once
#include <cstdlib>
#include <iostream>
#include <experimental/source_location>

namespace Sneep {
namespace debug {

#if !defined(NDEBUG)
template <typename T>
void print(const T var, const std::experimental::source_location loc = std::experimental::source_location::current()) {
  std::clog << "[" << loc.file_name() << ":" << loc.line() << "] in ("
            << loc.function_name() << "): " << var << std::endl;
}

template <typename T>
[[noreturn]]
void error(const T var, const std::experimental::source_location loc = std::experimental::source_location::current()) {
  std::cerr << "[" << loc.file_name() << ":" << loc.line() << "] in ("
            << loc.function_name() << "): " << var << std::endl;
  std::exit(1);
}
#else
void print(const T var, const std::experimental::source_location loc = std::experimental::source_location::current()) {}
void error(const T var, const std::experimental::source_location loc = std::experimental::source_location::current()) {}
#endif

}
}
