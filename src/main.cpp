#include <iostream>

#include "./types.h"
#include "./bitboard.h"
#include "./position.h"

int main(int argc, char **argv) {
  using namespace Sneep; // Teehee

  std::cout
    << "Debug Info:\n---------------"
    << "\nThrowErrors: " << ThrowErrors
    << "\nThrowExtraErrors: " << ThrowExtraErrors << std::endl;

  Position p;
  std::cout << p.piece_on(A1).print() << std::endl;
  std::cout << p << std::endl;
  return 0;
}
