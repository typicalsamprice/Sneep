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
  while (true) {
    std::string f;
    std::cout << "FEN: ";
    std::getline(std::cin, f);

    if (f.length() == 0)
      break;

    Position p(f);
    std::cout << p << std::endl;
  }


  return 0;
}
