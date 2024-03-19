#include <iostream>

#include "./types.h"
#include "./bitboard.h"
#include "./position.h"
#include "./perft.h"
#include "movegen.h"

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

  p.do_move(Move(E2, E3));
  p.do_move(Move(G8, H6));
  p.do_move(Move(F1, B5));

  std::cout << p << std::endl;
  std::cout << "Total Searched: " << perft<true>(p, 1) << std::endl;

  return 0;
}
