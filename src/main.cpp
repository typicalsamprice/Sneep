#include <iostream>

#include "./types.h"
#include "./bitboard.h"
#include "./position.h"

#warning Get rid of TODO macro

int main(int argc, char **argv) {
  using namespace Sneep; // Teehee

  std::clog << std::boolalpha;
  std::cout << std::boolalpha
    << "Debug Info:\n---------------"
    << "\nAssertionsEnabled: " << AssertionsEnabled
    << "\nThrowErrors: " << ThrowErrors
    << "\nThrowExtraErrors: " << ThrowExtraErrors << std::endl;

  Position p;
  std::cout << p << std::endl;
  return 0;
}
