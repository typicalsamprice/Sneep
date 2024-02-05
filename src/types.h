#ifndef TYPES_H_
#define TYPES_H_

#include <cassert>
#include <cstdint>

#include <bit>
#include <bitset>
#include <iostream>

namespace Sneep {

typedef uint64_t Bitboard;

enum File { File_A, File_B, File_C, File_D, File_E, File_F, File_G, File_H };
enum Rank {
  Rank_One,
  Rank_Two,
  Rank_Three,
  Rank_Four,
  Rank_Five,
  Rank_Six,
  Rank_Seven,
  Rank_Eight
};

// clang-format off
// These squares use LERF encoding: https://www.chessprogramming.org/Square_Mapping_Considerations#Little-Endian_Rank-File_Mapping
enum Square {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
  NO_SQUARE
};
// clang-format on

enum Color { White, Black };

enum Direction { DirN, DirS, DirE, DirW, DirNE, DirNW, DirSE, DirSW };

// Just squares + file/rank things
constexpr Square operator+(File f, Rank r) {
  return Square(int(f) + (int(r) << 3));
}
constexpr File file_of(Square s) { return File(int(s) & 7); }
constexpr Rank rank_of(Square s) {
  assert(s <= NO_SQUARE);
  // This is why UB happens! No other checks, it's costly :)
  return Rank(int(s) >> 3);
}

constexpr bool is_ok(Square s) {
  assert(s <= NO_SQUARE); // Maybe catch some UB in debug modes
  return s < NO_SQUARE;
}

// Bitboard stuff

}// namespace Sneep

#endif // TYPES_H_
