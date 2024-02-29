#include "movegen.h"
#include "bitboard.h"
#include "types.h"

namespace Sneep {

Bitboard BishopST[64][4] = {};
Bitboard RookST[64][4] = {};

static inline void add_promos(MoveList &moves, const Square from, const Square to) {
  moves.push_back(Move(from, to, Promotion, Knight));
  moves.push_back(Move(from, to, Promotion, Bishop));
  moves.push_back(Move(from, to, Promotion, Rook));
  moves.push_back(Move(from, to, Promotion, Queen));
}

template <GenType T>
MoveList generate_moves(const Position &pos) {
  constexpr bool DoCaps = T == Legal || T == Captures;

  const Color us = pos.to_move();
  const Bitboard pawns = pos.pieces(us, Pawn);
  Bitboard seventh = pawns & bb_from(relative_to(Rank_7, us));
  const Bitboard rest = pawns ^ seventh;

  Bitboard up = (rest << pawn_push(us)) &~ pos.pieces();
  Bitboard up2 = ((up & bb_from(relative_to(Rank_3, us))) << pawn_push(us)) &~ pos.pieces();

  Bitboard attacksE = (up << DirE) &~ mask_for(DirE) & pos.pieces(~us);
  Bitboard attacksW = (up << DirW) &~ mask_for(DirW) & pos.pieces(~us);
  MoveList moves;

  while (up) {
    Square s = pop_lsb(up);
    // Subtraction is fine, since we know that square must exist
    // due to the fact we shifted *from* it.
    Move m(s - pawn_push(us), s);
    moves.push_back(m);
  }
  while (up2) {
    Square s = pop_lsb(up2);
    // Subtraction is fine, since we know that square must exist
    // due to the fact we shifted *from* it.
    Move m(s - 2 * pawn_push(us), s);
    moves.push_back(m);
  }

  if constexpr (!DoCaps)
    goto SkipPawnCaps;

  while (attacksE) {
    Square s = pop_lsb(attacksE);
    Move m(s - pawn_push(us) - DirE, s);
    moves.push_back(m);
  }
  while (attacksW) {
    Square s = pop_lsb(attacksW);
    Move m(s - pawn_push(us) - DirW, s);
    moves.push_back(m);
  }


  // Generate EP
  if (/* DoCaps && *//* Technically no change due to the goto, but may help with no runtime loss (DoCaps is constexpr)  */ pos.ep() != NO_SQUARE)  {
    Bitboard caps = pawn_attacks(pos.ep(), ~us) & pawns;
    while (caps) {
      Square from = pop_lsb(caps);
      Move m(from, pos.ep(), EnPassant);
      moves.push_back(m);
    }
  }

SkipPawnCaps:

  Bitboard promup = seventh << pawn_push(us) &~ pos.pieces();
  Bitboard attackE = seventh << pawn_push(us) << DirE & pos.pieces(~us);
  Bitboard attackW = seventh << pawn_push(us) << DirW & pos.pieces(~us);

  while (promup) {
    Square s = pop_lsb(promup);
    Square f = s - pawn_push(us);
    assert(is_ok(f));
    add_promos(moves, f, s);
  }
  if (!DoCaps)
    goto SkipCapPromoGen;

  while (attackE) {
    Square s = pop_lsb(attackE);
    Square f = s - DirE - pawn_push(us);
    assert(is_ok(f));
    add_promos(moves, f, s);
  }
  while (attackW) {
    Square s = pop_lsb(attackW);
    Square f = s - DirW - pawn_push(us);
    assert(is_ok(f));
    add_promos(moves, f, s);
  }

SkipCapPromoGen:
  const Bitboard maskoff = DoCaps ? ~pos.pieces(us) : ~pos.pieces();
  // Should be ALL possible pawn moves done now
  Bitboard knights = pos.pieces(us, Knight);
  while (knights) {
    Square s = pop_lsb(knights);
    assert(is_ok(s));
    Bitboard movs = knight_attacks(s) & maskoff;
    while (movs) {
      Square to = pop_lsb(movs);
      assert(is_ok(to));
      moves.push_back(Move(s, to));
    }
  }

  Bitboard rook_q = pos.pieces(us, Rook, Queen);
  Bitboard bish_q = pos.pieces(us, Bishop, Queen);

  while (bish_q)  {
    Square bq = pop_lsb(bish_q);
      assert(is_ok(bq));
    Bitboard atts = slider_attacks<Bishop>(bq, pos.pieces()) & maskoff;
    while (atts) {
      Square t = pop_lsb(atts);
      assert(is_ok(t));
      moves.push_back(Move(bq, t));
    }
  }
  while (rook_q)  {
    Square rq = pop_lsb(rook_q);
      assert(is_ok(rq));
    Bitboard atts = slider_attacks<Rook>(rq, pos.pieces()) & maskoff;
    while (atts) {
      Square t = pop_lsb(atts);
      assert(is_ok(t));
      moves.push_back(Move(rq, t));
    }
  }

  assert(pos.pieces(us, King));
  Square king = lsb(pos.pieces(us, King));
  assert(is_ok(king));
  Bitboard atts = king_attacks(king) & maskoff;
  while (atts) {
    Square t = pop_lsb(atts);
    assert(is_ok(t));
    moves.push_back(Move(king, t));
  }

  CastleRights cr = pos.castle_rights();
  CastleRights cs = us == White ? White_OO : Black_OO;
  CastleRights cl = us == White ? White_OOO : Black_OOO;

  if (contains(cr, cs)) {
    assert(king == relative_to(E1, us));
    Square rook = relative_to(H1, us);
    assert(pos.piece_on(rook) == Piece(Rook, us));
    Bitboard btw = bb_between(king, rook); // Not including `rook`
    if (!(btw & pos.pieces()))
      moves.push_back(Move(king, rook + DirW, Castle));
  }
  if (contains(cr, cl)) {
    assert(king == relative_to(E1, us));
    Square rook = relative_to(A1, us);
    assert(pos.piece_on(rook) == Piece(Rook, us));
    Bitboard btw = bb_between(king, rook);
    if (!(btw & pos.pieces()))
      moves.push_back(Move(king, rook + DirE + DirE, Castle));
  }

  return moves;
}


template MoveList generate_moves<Quiet>(const Position &pos);
template MoveList generate_moves<Captures>(const Position &pos);
template MoveList generate_moves<Checks>(const Position &pos);
#warning "Implement generate_moves<Legal>"

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

    pot &= ~ray(last_hit, dir);

    rv |= pot;
  }

  return rv;
}

// NOTE: Do not add more instantiations.
template Bitboard slider_attacks<Bishop>(const Square, const Bitboard);
template Bitboard slider_attacks<Rook>(const Square, const Bitboard);
template Bitboard slider_attacks<Queen>(const Square, const Bitboard);

} // namespace Sneep
