#include "debug.h"
#include <experimental/source_location>
#include <iostream>

#if !defined(NDEBUG)
void Sneep::debug::dassert(const bool cond,
                           const std::experimental::source_location loc) {
  if (!cond) {
    std::clog << "[" << loc.file_name() << ":" << loc.line() << "] in ("
              << loc.function_name() << "): Debug Assertion Failed."
              << std::endl;
    std::exit(1);
  }
}
#else
void Sneep::debug::dassert(const bool cond,
                           const std::experimental::source_location loc) {}
#endif
