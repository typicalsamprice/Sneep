#ifndef BITBOARD_H_
#define BITBOARD_H_
#include "./types.h"

namespace Sneep {

void pretty_print(Bitboard b);

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
  return Square(__builtin_ctzll(b));
}

// Misc. Bitboard stuff
constexpr int popcnt(Bitboard b) {
  // TODO: Is this fast? Otherwise use __builtin_popcountll
  return std::bitset<64>(b).size();
}

constexpr bool more_than_one(Bitboard b) { return b & (b - 1); }
constexpr bool without_lsb(Bitboard b) { return b & (b - 1); }

}

#endif // BITBOARD_H_
