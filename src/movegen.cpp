#include "movegen.h"
#include "types.h"

namespace Sneep {

Bitboard BishopST[64][4] = {};
Bitboard RookST[64][4] = {};

Bitboard ray(const Square s, const Direction d) {
  int i = indexify<true>(d);
  if (i >= 4)
    return BishopST[s][index(d)];
  return RookST[s][index(d)];
}

// This should be somewhat faster, hence templated.
template <PieceT PT>
Bitboard slider_attacks(const Square s, const Bitboard occ) {
  if (PT == Queen)
    return slider_attacks<Bishop>(s, occ) | slider_attacks<Rook>(s, occ);

  Bitboard rv = 0;

  Direction bishDirs[4] = { DirNE, DirSE, DirSW, DirNW };
  Direction rookDirs[4] = { DirN, DirS, DirE, DirW };
  for (const Direction dir : (PT == Bishop ?  bishDirs : rookDirs)) {
    Bitboard pot = ray(s, dir);

    Bitboard blockers = pot & occ;

    if (!blockers) {
      rv |= pot;
      continue;
    }

    Square last_hit;
    if (lsb(blockers) > s)
      last_hit = lsb(blockers);
    else
      last_hit = msb(blockers);

    Bitboard rem = ray(last_hit, dir);
    pot ^= rem;

    rv |= pot;
  }

  return rv;
#undef ST
}

// NOTE: Do not add more instantiations.
template Bitboard slider_attacks<Bishop>(const Square, const Bitboard);
template Bitboard slider_attacks<Rook>(const Square, const Bitboard);
template Bitboard slider_attacks<Queen>(const Square, const Bitboard);

} // namespace Sneep
