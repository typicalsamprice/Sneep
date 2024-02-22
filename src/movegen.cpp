#include "movegen.h"
#include "types.h"

namespace Sneep {

Bitboard BishopST[64][4] = {};
Bitboard RookST[64][4] = {};

// This should be somewhat faster, hence templated.
template <PieceT PT>
Bitboard slider_attacks(const Square s, const Bitboard occ) {
  if (!is_slider(PT)) {
    if (ThrowErrors)
      debug::error("Invalid PT templated for slider_attacks<PT>");
    return 0;
  }

  if (PT == Queen)
    return slider_attacks<Bishop>(s, occ) | slider_attacks<Rook>(s, occ);

  Bitboard rv = 0;

  Direction bishDirs[4] = { DirNE, DirSE, DirSW, DirNW };
  Direction rookDirs[4] = { DirN, DirS, DirE, DirW };

  Bitboard *ST = PT == Bishop ? BishopST[s] : RookST[s];

  for (const Direction dir : (PT == Bishop ?  bishDirs : rookDirs)) {
    Bitboard pot = ST[indexify<false>(dir)];
    Bitboard blocking = pot & occ;
  }

  return rv;
}

} // namespace Sneep
