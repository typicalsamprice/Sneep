#pragma once
#include "./types.h"
#include <bit>

namespace Sneep {

extern Bitboard PawnMoves[64][2];
extern Bitboard KnightMoves[64];
extern Bitboard KingMoves[64];

void initialize_bitboards();
void pretty_print(Bitboard b);

inline Bitboard operator<<(Bitboard b, Direction d) {
  assert(int(d) < 64 && int(d) > -64);
  return int(d) >= 0 ? (b << int(d)) : (b >> -int(d));
}


// Creation from different elements
constexpr Bitboard bb_from(Square s) {
  assert(is_ok(s));
  return Bitboard(1) << int(s);
}
constexpr Bitboard bb_from(File f) {
  assert(int(f) < 8);
  return Bitboard(0x0101010101010101) << int(f);
}
constexpr Bitboard bb_from(Rank r) {
  assert(int(r) < 8);
  return Bitboard(0xFF) << (int(r) * 8);
}

// Serializing
constexpr Square lsb(Bitboard b) {
  assert(b); // Otherwise it is just NO_SQUARE
  return Square(std::countr_zero(b));
}

constexpr Square msb(Bitboard b) {
  assert(b);
  return Square(std::countl_zero(b));
}

// Misc. Bitboard stuff
constexpr int popcnt(Bitboard b) {
  return std::popcount(b);
}

// Yes, these are the same except for return type. It's on purpose.
constexpr bool more_than_one(Bitboard b) { return b & (b - 1); }
constexpr Bitboard without_lsb(Bitboard b) { return b & (b - 1); }

constexpr Bitboard FileAH_BB = bb_from(File_A) | bb_from(File_H);
constexpr Bitboard Rank18_BB = bb_from(Rank_1) | bb_from(Rank_8);
constexpr Bitboard BordersBB = FileAH_BB | Rank18_BB;
constexpr Bitboard borders_minus(Square s) {
  assert(is_ok(s));
  return (FileAH_BB &~ bb_from(file_of(s))) | (Rank18_BB &~ bb_from(rank_of(s)));
}
}
