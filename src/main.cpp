#include <iostream>

#include "./types.h"
#include "./bitboard.h"
#include "./position.h"
#include "./perft.h"

#if !defined(__cpp_lib_bitops) || __cpp_lib_bitops < 201907L
#error "Bit operations not defined!"
#error "Die"
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

  //p.do_move(Move(B2, B3));

  std::cout << p << std::endl;
  std::cout << perft<true>(p, 3) << std::endl;

  return 0;
}
