#include <iostream>

#include "./types.h"
#include "./bitboard.h"
#include "./position.h"
#include "./movegen.h"

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

  Move m(E2, E4);
  std::cout << p.is_legal(m, true);

  return 0;
}
