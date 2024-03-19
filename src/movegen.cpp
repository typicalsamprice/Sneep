#include "movegen.h"
#include "bitboard.h"
#include "types.h"

#include <algorithm>

namespace Sneep {

Bitboard BishopST[64][4] = {};
Bitboard RookST[64][4] = {};

static inline void add_promos(MoveList &moves, const Square from,
                              const Square to) {
  moves.push_back(Move(from, to, Promotion, Knight));
  moves.push_back(Move(from, to, Promotion, Bishop));
  moves.push_back(Move(from, to, Promotion, Rook));
  moves.push_back(Move(from, to, Promotion, Queen));
}

template <GenType T> MoveList generate_moves(const Position &pos) {
  if constexpr (T == Legal)
    debug::error("Cannot pass GenType::Legal");

  const Color us = pos.to_move();
  const Color them = ~us;
  const Square king = pos.king_of(us);
  const Direction up = pawn_push(us);
  const Bitboard rank7BB = us == White ? bb_from(Rank_7) : bb_from(Rank_2);
  const Bitboard rank3BB = us == White ? bb_from(Rank_3) : bb_from(Rank_6);

  const bool inCheck = pos.in_check();

  const Bitboard empty = ~pos.pieces();
  const Bitboard takeable = pos.pieces(them);

  const Bitboard pawns = pos.pieces(us, Pawn);
  Bitboard pawnsRank7 = pawns & rank7BB;
  Bitboard pawnsRest = pawns ^ pawnsRank7;

  MoveList moves;

  Bitboard up1 = (pawnsRest << up) & empty;
  Bitboard up2 = ((up1 & rank3BB) << up) & empty;

  while (up1) {
    Square t = pop_lsb(up1);
    moves.push_back(Move(t - up, t));
  }
  while (up2) {
    Square t = pop_lsb(up2);
    moves.push_back(Move(t - up - up, t));
  }

  if (pawnsRank7) {
    Bitboard p_up = pawnsRank7 << up & empty;
    Bitboard p_e = pawnsRank7 << up << DirE & takeable &~ bb_from(File_A);
    Bitboard p_w = pawnsRank7 << up << DirW & takeable &~ bb_from(File_H);

    while (p_up) {
      Square t = pop_lsb(p_up);
      add_promos(moves, t - up, t);
    }
    while (p_e) {
      Square t = pop_lsb(p_e);
      add_promos(moves, t - up - DirE, t);
    }
    while (p_w) {
      Square t = pop_lsb(p_w);
      add_promos(moves, t - up - DirW, t);
    }
  }

  Bitboard ae = (pawnsRest << up << DirE) & takeable &~ bb_from(File_A);
  Bitboard aw = (pawnsRest << up << DirW) & takeable &~ bb_from(File_H);

  while (ae) {
    Square t = pop_lsb(ae);
    moves.push_back(Move(t - up - DirE, t));
  }
  while (aw) {
    Square t = pop_lsb(aw);
    moves.push_back(Move(t - up - DirW, t));
  }

  if (pos.ep() != NO_SQUARE) {
    assert(relative_to(rank_of(pos.ep()), us) == relative_to(Rank_6, us));
    Bitboard potent = pawn_attacks(pos.ep(), them) & pawnsRest;
    assert(potent);
    while (potent)
      moves.push_back(Move(pop_lsb(potent), pos.ep(),EnPassant));
  }

  // END PAWN GEN
  Bitboard target = ~pos.pieces(us);

  Bitboard knights = pos.pieces(us, Knight);
  while (knights) {
    Square kn = pop_lsb(knights);
    assert(pos.piece_on(kn) == Piece(Knight, us));
    Bitboard atts = attacks_for(pos.piece_on(kn), kn, pos.pieces()) & target;
    while (atts)
      moves.push_back(Move(kn, pop_lsb(atts)));
  }
  Bitboard bqs = pos.pieces(us, Bishop, Queen);
  while (bqs) {
    Square bq = pop_lsb(bqs);
    Bitboard atts = attacks_for(Piece(Bishop, us), bq, pos.pieces()) & target;
    while (atts)
      moves.push_back(Move(bq, pop_lsb(atts)));
  }
  Bitboard rqs = pos.pieces(us, Rook, Queen);
  while (rqs) {
    Square rq = pop_lsb(rqs);
    Bitboard atts = attacks_for(Piece(Rook, us), rq, pos.pieces()) & target;
    while (atts)
      moves.push_back(Move(rq, pop_lsb(atts)));
  }

  Bitboard katts = attacks_for(pos.piece_on(king), king, pos.pieces()) & target;
  while (katts)
    moves.push_back(Move(king, pop_lsb(katts)));

  const CastleRights csr = pos.castle_rights();
  if (contains(csr, kingside(us)) && !(bb_between(king, relative_to(H1, us)) & pos.pieces())) {
    assert(king == relative_to(E1, us));
    moves.push_back(Move(king, relative_to(G1, us), Castle));
  }
  if (contains(csr, queenside(us)) && !(bb_between(king, relative_to(A1, us)) & pos.pieces())) {
    assert(king == relative_to(E1, us));
    moves.push_back(Move(king, relative_to(C1, us), Castle));
  }

  return moves;
}

template MoveList generate_moves<Captures>(const Position &pos);
template MoveList generate_moves<All>(const Position &pos);

template <> MoveList generate_moves<Legal>(const Position &pos) {
  MoveList moves = generate_moves<All>(pos);

  // TODO Actually not check every single one
  moves.erase(
      std::remove_if(moves.begin(), moves.end(),
                     [&pos](Move m) {
                       // Check if we need the legality check
                       if (pos.moved_piece(m).type == King || pos.in_check())
                         return !pos.is_legal(m, false);
                       else if (pos.blockers(pos.to_move()) & bb_from(m.from()))
                         return !pos.is_legal(m, false);

                       return false;
                     }),
      moves.end());

  return moves;
}

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

  Direction bishDirs[4] = {DirNE, DirSE, DirSW, DirNW};
  Direction rookDirs[4] = {DirN, DirS, DirE, DirW};
  for (const Direction dir : (PT == Bishop ? bishDirs : rookDirs)) {
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
