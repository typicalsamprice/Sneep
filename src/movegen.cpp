#include "movegen.h"
#include "types.h"

namespace Sneep {

Bitboard BishopST[64][4] = {};
Bitboard RookST[64][4] = {};

// This should be somewhat faster, hence templated.
template <PieceT PT>
Bitboard slider_attacks(const Square s, const Bitboard occ) {
  if (PT == Queen)
    return slider_attacks<Bishop>(s, occ) | slider_attacks<Rook>(s, occ);

  Bitboard sb = bb_from(s);

  Bitboard rv = 0;

  Direction bishDirs[4] = { DirNE, DirSE, DirSW, DirNW };
  Direction rookDirs[4] = { DirN, DirS, DirE, DirW };

  Bitboard *ST = PT == Bishop ? BishopST[s] : RookST[s];

  for (const Direction dir : (PT == Bishop ?  bishDirs : rookDirs)) {
    Bitboard pot = ST[indexify<false>(dir)];
    Bitboard blocking = pot & occ;

    if (!blocking) {
      rv |= pot;
      continue;
    }

    // Remove the right ray
    Square concerned;
    if (blocking > sb) {
      concerned = lsb(blocking);
    } else {
      concerned = msb(blocking);
    }

    Bitboard fixed = pot ^ (PT == Bishop ? BishopST : RookST)[concerned][index(dir)];
    rv |= fixed;
  }

  return rv;
}

// NOTE: Do not add more instantiations.
template Bitboard slider_attacks<Bishop>(const Square, const Bitboard);
template Bitboard slider_attacks<Rook>(const Square, const Bitboard);
template Bitboard slider_attacks<Queen>(const Square, const Bitboard);

} // namespace Sneep
