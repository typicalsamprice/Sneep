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

    PawnMoves[s][0] = make_pawn_attacks(s, White);
    PawnMoves[s][1] = make_pawn_attacks(s, Black);

    Bitboard king = PawnMoves[s][0] | PawnMoves[s][1];
    king |= (PawnMoves[s][1] << DirN) | (PawnMoves[s][0] << DirS); // Get the side-middle
    king |= bb_from(s) << pawn_push(White); // Vert-middle
    king |= bb_from(s) << pawn_push(Black);
    KingMoves[s] = king;

    Bitboard kn = 0;
    Bitboard b = bb_from(s);
    Bitboard nn = b << (2 * DirN);
    Bitboard ss = b << (2 * DirS);
    Bitboard ww = b << (2 * DirW);
    Bitboard ee = b << (2 * DirE);

    kn |= (nn << DirE | ss << DirE) &~ bb_from(File_A);
    kn |= (nn << DirW | ss << DirW) &~ bb_from(File_H);
    kn |= (ee | ww) << DirN;
    kn |= (ee | ww) << DirS;

    KnightMoves[s] = kn;

    Direction rook_dirs[]{DirN, DirS, DirE, DirW};
    Direction bishop_dirs[]{DirNE, DirSW, DirSE, DirNW};
    for (int i = 0; i < 4; ++i) {
      Bitboard rook_moves = 0;
      Bitboard b = bb_from(s);

      for (int j = 0; j < distance_to_edge(s, rook_dirs[i]); j++) {
        b <<= rook_dirs[i];
        rook_moves |= b;
      }

      RookST[s][indexify<false>(rook_dirs[i])] = rook_moves;
    }
    for (int i = 0; i < 4; ++i) {
      Bitboard bishop_moves = 0;
      Bitboard b = bb_from(s);

      for (int j = 0; j < distance_to_edge(s, bishop_dirs[i]); j++) {
        b <<= bishop_dirs[i];
        bishop_moves |= b;
      }

      BishopST[s][indexify<false>(bishop_dirs[i])] = bishop_moves;
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
