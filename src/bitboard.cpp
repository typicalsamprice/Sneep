#include <string>

#include "bitboard.h"
#include "types.h"
#include "movegen.h"

#include <iostream>

namespace Sneep {

Bitboard PawnMoves[64][2] = {};
Bitboard KnightMoves[64] = {};
Bitboard KingMoves[64] = {};
Bitboard BetweenBB[64][64] = {};
Bitboard LineBB[64][64] = {};

namespace {
inline Bitboard make_pawn_attacks(Square s, Color c) {
  assert(is_ok(s));
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
    Bitboard ss = b >> -2 * DirS;
    Bitboard ww = (b >> -2 * DirW) &~ (bb_from(File_H) | bb_from(File_G));
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
  for (Square s1 = A1; s1 <= H8; ++s1) // This happens AFTER the rest, since we need all the BishopST/RookST initialized
    for (Square s2 = A1; s2 <= H8; ++s2) {
      // Probably exists a better way to iterate over all the dirs, don't care rn though.
      if (s1 == s2) {
        BetweenBB[s1][s2] = 0;
        continue;
      }
      Bitboard bw = 0;
      Bitboard line = 0;
      Direction dirs[]{DirE, DirN, DirW, DirS, DirNE, DirNW, DirSE, DirSW};
      for (Direction d : dirs) {
        Bitboard r = ray(s1, d);
        if (r & bb_from(s2)) {
          // We know this is the "right" direction
          bw = r & ray(s2, ~d);
          line = ray(s2, ~d) | ray(s1, d);
          break;
        }
      }

      BetweenBB[s1][s2] = BetweenBB[s2][s1] = bw;
      LineBB[s1][s2] = LineBB[s2][s1] = line;
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
