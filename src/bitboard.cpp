#include <string>

#include "bitboard.h"
#include "types.h"
#include "movegen.h"

#include <iostream>

namespace Sneep {

Bitboard PawnMoves[64][2] = {};
Bitboard KnightMoves[64] = {};
Bitboard KingMoves[64] = {};

namespace {
inline Bitboard make_pawn_attacks(Square s, Color c) {
  assert(is_ok(s));
  if (rank_of(s) == Rank_8 || rank_of(s) == Rank_1)
      return 0; // Should NEVER matter, not dealing with that.
  Bitboard up = bb_from(s) << pawn_push(c);
  return ((up << DirE) & ~bb_from(File_A)) | ((up << DirW) & ~bb_from(File_H));
}
} // namespace

void initialize_bitboards() {
  for (Square sinc = A1; sinc <= H8; ++sinc) {
    const Square s = sinc;
    const Bitboard b = bb_from(s);

    PawnMoves[s][White] = make_pawn_attacks(s, White);
    PawnMoves[s][Black] = make_pawn_attacks(s, Black);

    Bitboard updown = (b << DirN) | (b << DirS);
    Bitboard kings = updown;
    kings |= ((updown | b) << DirE) &~ bb_from(File_A);
    kings |= ((updown | b) << DirW) &~ bb_from(File_H);
    KingMoves[s] = kings;

    Bitboard knights = 0;
    Bitboard nn = b << 2 * DirN;
    Bitboard ss = b << 2 * DirS;
    Bitboard ww = (b << 2 * DirW) &~ (bb_from(File_H) | bb_from(File_G));
    Bitboard ee = (b << 2 * DirE) &~ (bb_from(File_A) | bb_from(File_B));

    knights |= ww << DirN;
    knights |= ww << DirS;
    knights |= ee << DirN;
    knights |= ee << DirS;
    knights |= (nn << DirE) &~ bb_from(File_A);
    knights |= (nn << DirW) &~ bb_from(File_H);
    knights |= (ss << DirE) &~ bb_from(File_A);
    knights |= (ss << DirW) &~ bb_from(File_H);

    KnightMoves[s] = knights;

    Direction r_dirs[] = { DirN, DirS, DirW, DirE };
    Direction b_dirs[] = { DirNE, DirSE, DirNW, DirSW };
    for (const Direction d : r_dirs) {
      Bitboard mask = mask_for(d);
      Bitboard tb = b; // copy, to change around.
      Bitboard att = 0;

      while ((tb <<= d)) {
        tb &= ~mask;
        att |= tb;
      }
      RookST[s][index(d)] = att;
    }
    for (const Direction d : b_dirs) {
      Bitboard mask = mask_for(d);
      Bitboard tb = b; // copy, to change around.
      Bitboard att = 0;

      while (tb <<= d) {
        tb &= ~mask;
        att |= tb;
      }
      BishopST[s][index(d)] = att;
    }
  }
}

void pretty_print(const Bitboard b) {
  std::string sep("+---+---+---+---+---+---+---+---+\n");
  std::cout << sep;
  for (Rank r = Rank_8; r >= Rank_1; --r) {
    for (File f = File_A; f <= File_H; ++f) {
      Bitboard bit = bb_from(f + r) & b;
      std::cout << "| " << (bit ? "x" : " ") << " ";
    }
    std::cout << "|\n" << sep;
  }
}

} // namespace Sneep
