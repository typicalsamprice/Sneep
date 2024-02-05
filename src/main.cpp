#include <iostream>

#include "./types.h"
#include "./bitboard.h"

int main(int argc, char **argv) {
  using namespace Sneep; // Teehee

  Bitboard b = 0;
  Bitboard b2 = bb_from(A1) | bb_from(H2);

  pretty_print(b2);

  return 0;
}
