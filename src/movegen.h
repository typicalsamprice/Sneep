#pragma once
#include "types.h"
#include "bitboard.h"
#include "position.h"
#include <vector>

namespace Sneep {

extern Bitboard BishopST[64][4];
extern Bitboard RookST[64][4];

enum GenType {
  Legal,
  Captures,
  Checks,
  Quiet,
  All
};

typedef std::vector<Move> MoveList;

template <GenType> MoveList generate_moves(const Position &pos);

Bitboard ray(const Square s, const Direction d);
template <PieceT PT>
Bitboard slider_attacks(const Square s, const Bitboard occ);

inline Bitboard pawn_attacks(const Square s, const Color c) {
  assert(is_ok(s));
  return PawnMoves[s][c];
}

constexpr Bitboard knight_attacks(const Square s) {
  assert(is_ok(s));
  return KnightMoves[s];
}

constexpr Bitboard king_attacks(const Square s) {
  assert(is_ok(s));
  return KingMoves[s];
}

inline Bitboard attacks_for(const Piece p, const Square f, const Bitboard occ) {
  Bitboard att;
  switch (p.type) {
    case Pawn:
      att = pawn_attacks(f, p.color);
      break;
    case Knight:
      att = knight_attacks(f);
      break;
    case Bishop:
      att = slider_attacks<Bishop>(f, occ);
      break;
    case Rook:
      att = slider_attacks<Rook>(f, occ);
      break;
    case Queen:
      att = slider_attacks<Queen>(f, occ);
      break;
    case King:
      att = king_attacks(f);
      break;
    default:
      debug::error("Invalid Piece type");
  }

  return att;
}

} // namespace Sneep
