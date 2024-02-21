#pragma once
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
#else
void print(const T var, const std::experimental::source_location loc = std::experimental::source_location::current()) {}
#endif

}
}
