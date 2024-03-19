#include "position.h"
#include "bitboard.h"
#include "movegen.h"
#include "types.h"
#include <cstring>
#include <ios>
#include <iostream>
#include <optional>
#include <sstream>

#include "debug.h"

namespace Sneep {
// I totally forgot how to do this operator overload to easily view the
// Position:
// https://github.com/official-stockfish/Stockfish/blob/master/src/position.cpp#L64
std::ostream &operator<<(std::ostream &os, const Position &pos) {
  std::string sep("+---+---+---+---+---+---+---+---+\n");
  os << sep;
  for (Rank r = Rank_8; r >= Rank_1; --r) {
    for (File f = File_A; f <= File_H; ++f) {
      Piece p = pos.piece_on(f + r);
      os << "| " << p.print() << " ";
    }
    os << "|\n" << sep;
  }
  return os;
}

bool Position::is_legal(const Move m, const bool checkPL) const {
  if (checkPL && !is_pseudo_legal(m))
    return false;

  Square from = m.from(), to = m.to();
  Color us = to_move();
  Color them = ~us;
  Square king = king_of(us);
  MoveT mt = m.type();

  Piece movp = moved_piece(m);
  assert(is_ok(movp));

  if (from == king && movp != Piece(King, us))
    return false;

  if (state->checkers) {
    if (ThrowErrors && popcnt(state->checkers) > 2)
      debug::error("Somehow more than 2 checkers, aborting.");

    if (popcnt(state->checkers) == 2) {
      if (from != king)
        return false;
      goto AfterCheckersCheck;
    }

    // Only one checker here
    assert(state->checkers && popcnt(state->checkers) == 1);
    Square checker = lsb(state->checkers);
    Bitboard up_to_checker = bb_between(king, checker) | state->checkers;
    if (from != king) {
      // Have to take or interpose if not moving king.
      if (!(bb_from(to) & up_to_checker))
        return false;
    }
  }

AfterCheckersCheck:

  // moving into check.
  if (from == king) {
    Bitboard mask = mt == Castle ? bb_between(from, to) | bb_from(to) : 0;
    if (attacks_to(to, pieces() ^ bb_from(from) ^ mask) & pieces(them))
      return false;
  }

  if (state->blockers[us] & bb_from(from)) {
    Bitboard pins = state->pinners[them];
    // Valid, since a king cannot be its own blocker.
    Bitboard pin_in_line = bb_line(from, king) & pins;
    assert(pin_in_line);

    Square pinner = NO_SQUARE;
    while (pin_in_line) {
      Square pin = pop_lsb(pin_in_line);
      if (bb_between(pin, king) & bb_from(from)) {
        pinner = pin;
        break;
      }
    }
    assert(is_ok(pinner));
    if (!((bb_between(from, to) | bb_from(to)) & bb_from(pinner)))
      return false;
  }

  if (mt == EnPassant) {
    Square capture = file_of(to) + rank_of(from);
    Bitboard no_extra =
        pieces() ^ bb_from(from) ^ bb_from(to) ^ bb_from(capture);
    // Don't count attacks from the taken pawn.
    Bitboard atts =
        attacks_to(king, no_extra) & pieces(them) & ~bb_from(capture);
    if (atts)
      return false;
  } else if (mt == Castle) {
    Bitboard sqs = bb_between(from, to) | bb_from(to);
    // Cannot castle through OR into check.
    Bitboard bits = pieces() ^ bb_from(from);
    while (sqs) {
      Square check = pop_lsb(sqs);
      if (attacks_to(check, bits) & pieces(them))
        return false;
    }
  }

  return true;
}

bool Position::is_pseudo_legal(Move m) const {
  // Copied and transposed from my old project at
  // https://github.com/typicalsamprice/chess
  // since that is written in Rust, not C++.
  Square from = m.from();
  Square to = m.to();

  MoveT mt = m.type();
  Color us = to_move();
  Color them = ~us;

  Square king = king_of(us);

  if (from == to)
    return false;

  Piece movp = moved_piece(m);
  if (!is_ok(movp))
    return false;

  // [BEGIN] Check if pseudo-legal for this type of piece

  switch (movp.type) {
  case NO_TYPE:
  case ALL_TYPES:
    return false; // Shouldn't ever happen
    break;
  case Pawn:
    if (us == White && from > to)
      return false;
    else if (us == Black && from < to)
      return false;

    if (distance(from, to) > 2)
      return false;
    if (distance(from, to) == 2 && file_of(from) != file_of(to))
      return false;
    if (rank_of(from) == rank_of(to))
      return false;
    // The 2-step over is covered on all between-cross check below this section.
    if (empty(to)) {
      if (file_of(from) != file_of(to) || to != state->ep)
        return false;
    } else if (file_of(from) == file_of(to))
      return false;
    break;
  case Knight:
    if (!(knight_attacks(from) & bb_from(to)))
      return false;
    break;
  case Bishop:
    if (!(slider_attacks<Bishop>(from, pieces()) & bb_from(to)))
      return false;
    break;
  case Rook:
    if (!(slider_attacks<Rook>(from, pieces()) & bb_from(to)))
      return false;
    break;
  case Queen:
    if (!(slider_attacks<Queen>(from, pieces()) & bb_from(to)))
      return false;
    break;
  case King:
    if (distance(from, to) != 1)
      return false;
    break;
  }

  // [END] Check if pseudo-legal for this type of piece

  if (bb_between(from, to) & pieces())
    return false;

  Square capture_square = mt == EnPassant ? (file_of(to) + rank_of(from)) : to;
  std::optional<Piece> captured = captured_piece(m);

  if (captured) {
    Piece cap = captured.value();
    if (cap.color == us)
      return false;
    if (capture_square != to) {
      if (cap != Piece(Pawn, them))
        return false;
      if (movp != Piece(Pawn, us))
        return false;
    }
  }

  if (mt == Castle) {
    // Can't castle while in check.
    if (state->checkers)
      return false;

    const CastleRights cr = state->cr;
    const CastleRights full_for = us == White ? White_CR : Black_CR;
    if (!contains(cr, full_for))
      return false;
    CastleRights att;
    Square rf, rt;
    if (to == relative_to(G1, us)) {
      att = kingside(us);
      rf = relative_to(H1, us);
      // rt = relative_to(F1, us);
    } else if (to == relative_to(C1, us)) {
      att = queenside(us);
      rf = relative_to(A1, us);
      // rt = relative_to(D1, us);
    } else if (ThrowExtraErrors) {
      debug::error("Trying to castle via move to non-castling square.");
    } else {
      return false;
    }

    // Squares between king and rook must be empty.
    Bitboard between = bb_between(from, rf) & pieces();
    if (between)
      return false;
  }

  return true;
}

void Position::do_move(Move m) {
  assert(!ThrowErrors || is_legal(m, false));

  State *newState = new State;
  std::memcpy(newState, state, sizeof(State));
  newState->prev = state;
  state = newState;

  state->halfmoves++;

  Color us = to_move();
  Color them = ~us;
  Square from = m.from();
  Square to = m.to();
  MoveT mt = m.type();
  Piece prom = Piece(m.promo(), us);

  Piece movp = moved_piece(m);
  assert(is_ok(movp));

  std::optional<Piece> cap_opt = captured_piece(m);
  Piece capp;
  if (mt == EnPassant) {
    assert(!cap_opt);
    capp = Piece(Pawn, them);
  } else {
    capp = cap_opt.value_or(Piece());
  }

  if (is_ok(capp)) {
    Square cap_on = mt == EnPassant ? (file_of(to) + rank_of(from)) : to;
    assert(piece_on(cap_on) == capp);

    Piece remp = remove_piece(cap_on);
    assert(remp == capp);

    state->captured = std::make_optional(remp);

    if (capp.type == Rook) {
      // Take away castle rights!
      if (file_of(cap_on) == File_A && contains(state->cr, queenside(them)))
        state->cr -= queenside(them);
      else if (file_of(cap_on) == File_H && contains(state->cr, kingside(them)))
        state->cr -= kingside(them);
    }

    state->halfmoves = 0;
  }

  if (mt == Castle) {
    assert(movp.type == King);
    assert(from == relative_to(E1, us));
    assert(distance(from, to) == 2);
    assert(rank_of(from) == rank_of(to));

    CastleRights castle = from > to ? queenside(us) : kingside(us);
    assert(contains(state->cr, castle));
    do_castle<true>(castle);
    // TODO
  } else {
    remove_piece(from);
    put_piece(movp, to);
  }

  // OPT: Skip check, always set?
  if (state->ep != NO_SQUARE)
    state->ep = NO_SQUARE;

  if (movp.type == Pawn) {
    state->halfmoves = 0;
    Square possible = file_of(from) + relative_to(Rank_3, us);
    // Only set if necessary, can optimize movegen with a flag
    // OPT: Call flag `OptimizeEPMovegen`?
    if ((distance(from, to) == 2) &&
        (pawn_attacks(possible, us) & pieces(them, Pawn))) {
      state->ep = possible;
    } else if (mt == Promotion) {
      assert(rank_of(to) == relative_to(Rank_8, us));
      // Remove the now-moved piece, then replace with promoted piece
      Piece pawn_removed = remove_piece(to);
      assert(pawn_removed == Piece(Pawn, us));
      put_piece(prom, to);
    }
  }

  if (movp.type == King) {
    state->cr -= kingside(us);
    state->cr -= queenside(us);
  } else if (movp.type == Rook) {
    // OPT: Skip `contains` check and always remove?
    if (file_of(from) == File_A && contains(state->cr, queenside(us)))
      state->cr -= queenside(us);
    else if (file_of(from) == File_H && contains(state->cr, kingside(us)))
      state->cr -= kingside(us);
  }

  state->captured = cap_opt;
  side_to_move = them;
  _fullmoves++;
  update_state();
  hist.push_back(m);
}

void Position::undo_move() {
  // Also "stolen" from my own Rust implementation
  Move m = hist.back();
  hist.pop_back();

  side_to_move = ~side_to_move;

  Color us = to_move();
  Color them = ~us;
  Square from = m.from();
  Square to = m.to();
  MoveT mt = m.type();

  Piece movp = piece_on(to);
  assert(is_ok(movp));
  assert(!state->captured.has_value() || state->captured.value().type != King);

  // Quickly replace the promoted piece with old pawn.
  if (mt == Promotion) {
    assert(rank_of(to) == relative_to(Rank_8, us));
    assert(movp.type == m.promo());
    remove_piece(to);
    put_piece(Piece(Pawn, us), to);
  }

  if (mt == Castle) {
    CastleRights ct = (file_of(to) == File_G) ? kingside(us) : queenside(us);
    assert(ct != queenside(us) || file_of(to) == File_C);
    do_castle<false>(ct);
  } else {
    Piece p = remove_piece(to);
    assert(p == movp);
    put_piece(movp, from);

    if (state->captured.has_value()) {
      Square back_on = mt == EnPassant ? (file_of(to) + rank_of(from)) : to;
      assert(back_on == to ||
             state->captured == std::make_optional(Piece(Pawn, them)));
      put_piece(state->captured.value(), back_on);
    }
  }

  State *prev = state->prev;
  free(state);
  state = prev;
  _fullmoves--;
}

template <bool Do> void Position::do_castle(const CastleRights castle) {
  Color us = to_move();
  Square kf = relative_to(E1, us), kt, rf, rt;
  if (castle == kingside(us)) {
    kt = relative_to(G1, us);
    rf = relative_to(H1, us);
    rt = relative_to(F1, us);
  } else {
    assert(castle == queenside(us));
    kt = relative_to(C1, us);
    rf = relative_to(A1, us);
    rt = relative_to(D1, us);
  }

  assert(!Do || king_of(us) == kf);
  assert(!Do || contains(state->cr, castle));

  Square rook_rem = Do ? rf : rt;
  Square rook_rep = Do ? rt : rf;
  Square king_rem = Do ? kf : kt;
  Square king_rep = Do ? kt : kf;

  Piece king = remove_piece(king_rem);
  assert(king == Piece(King, us));
  Piece rook = remove_piece(rook_rem);
  assert(rook == Piece(Rook, us));
  put_piece(king, king_rep);
  put_piece(rook, rook_rep);
}

Position::Position(std::string fenString) {
  // Should this throw an error when a failure happens? THROW_EXTRA_ERRS
  // controls this, otherwise gives invalid Position (use at own risk!)
  std::istringstream fen(fenString);

  // This is just setting up the empty memory to
  // not screw me over later.
  std::memset(this, 0, sizeof(Position));
  for (int i = 0; i < 64; i++)
    squares[i] = Piece();

  state = new State;
  std::memset(state, 0, sizeof(State));

  Square s = A8;
  fen >> std::noskipws;
  char tok;
  while ((fen >> tok) && !isspace(tok)) {
    if (tok == '/') {
      if (file_of(s) != File_A) { // It should have wrapped by now, right?
        if (ThrowErrors)
          assert(0 && "Rank overflow");
        return;
      }

      assert(!ThrowExtraErrors || file_of(s) == File_A);

      s -= 2 * DirN;
      continue;
    } else if (tok < '9' && tok > '0') {
      File of = file_of(s); // ONLYFANS??!?!
      s += (tok - '0') * DirE;
      if (file_of(s) < of && ThrowErrors)
        assert(0 && "Rank overflow");
      continue;
    }

    Piece p(tok);

    // debug::print(s);
    // debug::print(p);
    // debug::print(tok);

    // This obviously isn't a bug (anymore), but I feel SO smart immediately
    // realizing the segfault was a) In the Position constructor b) Specifically
    // in this method call c) It was actually above, construction from a [char]
    // wasn't precise

    // Mother Freaking Heck! There was another bug I *thought* was on this line
    // (unrelated to the segfault) and it was because C++ doesn't freaking allow
    // constructor chaining! Who would have thought. Now, see how I found out:
    // https://stackoverflow.com/questions/187640/default-parameters-with-c-constructors
    // Yeah, that's why it has a default constructor! :) Wasted only an hour or
    // three on this.
    put_piece(p, s);
    ++s;
  }

  fen >> tok;
  side_to_move = (tok == 'w' ? White : Black);

  if (ThrowExtraErrors)
    if (tok != 'w' && tok != 'b')
      assert(0 && "Invalid Color to move in FEN.");

  fen >> tok;
  while ((fen >> tok) && !isspace(tok)) {
    switch (tok) {
    case 'K':
      state->cr |= White_OO;
      break;
    case 'Q':
      state->cr |= White_OOO;
      break;
    case 'k':
      state->cr |= Black_OO;
      break;
    case 'q':
      state->cr |= Black_OOO;
      break;
    default:
      debug::error(tok);
    }
  }

  fen >> tok;
  if (tok != '-') {
    if (ThrowErrors)
      assert(tok >= 'a' && tok <= 'h');
    File epf = File(tok - 'a');
    fen >> tok;
    if (ThrowErrors)
      assert(tok >= '1' && tok <= '9');
    Rank epr = Rank(tok - '1');
    state->ep = epf + epr;
  } else {
    // Just to be sure
    state->ep = NO_SQUARE;
  }

  fen >> std::skipws >> state->halfmoves >> _fullmoves;

  if (ThrowExtraErrors && !state->is_ok())
    debug::error("Invalid State produced by FEN");
}

void Position::put_piece(Piece p, Square s) {
  debug::dassert(empty(s));

  Bitboard b = bb_from(s);
  colors_bb[p.color] |= b;
  pieces_bb[p.type] |= b;
  pieces_bb[ALL_TYPES] |= b;
  squares[s] = p;
  // Should be all?? TODO Bugtest/Perftest to catch it
}
Bitboard Position::attacks_to(Square s, Bitboard occ) const {
  Bitboard rv = 0;
  // Compiler should auto-reuse that value, since this is all `const` wrt
  // Position.
  Bitboard rq_att = sliders_to(s, occ) & pieces(Rook, Queen);
  Bitboard bq_att = sliders_to(s, occ) & pieces(Bishop, Queen);

  rv |= rq_att | bq_att;

  Bitboard wpawns = pieces(White, Pawn) & pawn_attacks(s, Black);
  Bitboard bpawns = pieces(Black, Pawn) & pawn_attacks(s, White);

  Bitboard knights = pieces(Knight) & knight_attacks(s);
  Bitboard king = pieces(King) & king_attacks(s);

  rv |= wpawns | bpawns;
  rv |= knights | king;

  rv |= sliders_to(s, occ);

  return rv;
}

Bitboard Position::sliders_to(Square s, Bitboard occ) const {
  Bitboard rv = 0;

  rv |= slider_attacks<Bishop>(s, occ) & pieces(Bishop, Queen);
  rv |= slider_attacks<Rook>(s, occ) & pieces(Rook, Queen);

  return rv;
}

void Position::setup_state() {
  Color us = to_move();
  Color them = ~us;

  Square king = king_of(us);

  state->blockers[0] = state->blockers[1] = state->pinners[0] =
      state->pinners[1] = state->checkers = 0;
  // Normally, you would also reset the move-to-deliver-check squares,
  // but honestly I don't need a Position::gives_check(const Move m) const;
  // method since we aren't doing super intelligent stuff right now.
  // TODO ^^^

  state->checkers = attacks_to(king) & pieces(them);

  state->blockers[White] = calc_pinners_blockers(state->pinners[Black], White);
  state->blockers[Black] = calc_pinners_blockers(state->pinners[White], Black);

  // TODO Re-calculate check squares for piece types here.
}
void Position::update_state() {
  setup_state(); // Do it every time, I don't have time for
                 // incremental updates right now.
}

Bitboard Position::calc_pinners_blockers(Bitboard &pinners, const Color us) {
  Bitboard blockers = 0;
  pinners = 0;

  const Square king = king_of(us);
  Bitboard atts = sliders_to(king, 0) & pieces(~us);

  while (atts) {
    Square slider = pop_lsb(atts);
    // They can have their own piece be a blocker, it's fine!
    Bitboard l = bb_line(slider, king) & pieces();
    if (l) {
      if (more_than_one(l))
        continue;
      blockers |= l;
      pinners |= bb_from(slider);
    }
  }

  return blockers;
}

} // namespace Sneep
