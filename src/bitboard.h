#pragma once
#include "./types.h"
#include <bit>

namespace Sneep {

extern Bitboard PawnMoves[64][2];
extern Bitboard KnightMoves[64];
extern Bitboard KingMoves[64];
extern Bitboard BetweenBB[64][64];

void initialize_bitboards();
void pretty_print(Bitboard b);

inline Bitboard operator<<(Bitboard b, Direction d) {
  assert(abs(int(d)) < 64);
  return int(d) >= 0 ? (b << int(d)) : (b >> abs(int(d)));
}
constexpr Bitboard operator<<=(Bitboard &b, Direction d) {
  return b = b << d;
}

// Symmetric wrt a and b, so it shouldn't be hard...right?
constexpr Bitboard bb_between(const Square a, const Square b) {
  assert(is_ok(a) && is_ok(b));
  return BetweenBB[a][b];
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
  return Square(63 - std::countl_zero(b));
}

// Misc. Bitboard stuff
constexpr int popcnt(Bitboard b) {
  return std::popcount(b);
}

constexpr Bitboard mask_for(Direction d) {
  // So you can make sure you aren't overflowing.
  return (d == DirE | d == DirNE | d == DirSE) ? bb_from(File_A)
    : (d == DirW | d == DirNW | d == DirSW) ? bb_from(File_H)
    : 0;
}

// Yes, these are the same except for return type. It's on purpose.
constexpr bool more_than_one(Bitboard b) { return b & (b - 1); }
constexpr Bitboard without_lsb(Bitboard b) { return b & (b - 1); }
inline Square pop_lsb(Bitboard &b) {
  assert(b);
  Square s = lsb(b);
  b &= b - 1;
  return s;
}

}
