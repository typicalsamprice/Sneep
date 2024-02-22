#include <iostream>

#include "./types.h"
#include "./bitboard.h"
#include "./position.h"

#if !defined(__cpp_lib_bitops) || __cpp_lib_bitops < 201907L
#error "Bit operations not defined!"
#endif

int main(int argc, char **argv) {
  using namespace Sneep; // Teehee

  std::clog << std::boolalpha;
  std::cout << std::boolalpha
    << "Debug Info:\n---------------"
    << "\nAssertionsEnabled: " << AssertionsEnabled
    << "\nThrowErrors: " << ThrowErrors
    << "\nThrowExtraErrors: " << ThrowExtraErrors << std::endl;

  initialize_bitboards();

  Position p;
  std::cout << p << std::endl;
  return 0;
}
