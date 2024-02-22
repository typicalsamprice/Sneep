#pragma once
#include "./types.h"
#include "bitboard.h"

namespace Sneep {

extern Bitboard BishopST[64][4];
extern Bitboard RookST[64][4];

template <PieceT PT>
Bitboard slider_attacks(const Square s, const Bitboard occ);

inline Bitboard pawn_attacks(const Square s, const Color c) {
    assert(is_ok(s));
    if (ThrowExtraErrors && (rank_of(s) == Rank_1 || rank_of(s) == Rank_8))
        assert(0 && "Should NEVER have pawn on Rank 8 or 1");
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

}
