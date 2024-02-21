#include "position.h"
#include "types.h"
#include <ios>
#include <iostream>
#include <memory>
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

// {{{ Private Methods
bool Position::is_legal(Move m, bool assumePL) const {
  if (!assumePL && !is_pseudo_legal(m))
    return false;

  return true;
}
bool Position::is_legal(Move m) const { return is_legal(m, false); }
bool Position::is_pseudo_legal(Move m) const {
  Piece mp = moved_piece(m);
  if (!is_ok(mp) || mp.color != to_move) {
    return false; // Have to move correct color
  }

  Square to = m.to();
  Square from = m.from();

  return true;
}

void Position::do_move(Move m) {}
void Position::undo_move(Move m) {}
// }}}

// {{{ Public Methods
Position::Position(std::string fenString) {
  // Should this throw an error when a failure happens? THROW_EXTRA_ERRS
  // controls this, otherwise gives invalid Position (use at own risk!)
  std::istringstream fen(fenString);

  state = std::make_unique<State>();

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

    // std::cout << "Square: " << s << "\nPiece: " << p << "\nToken: " << tok << std::endl;

    // This obviously isn't a bug (anymore), but I feel SO smart immediately
    // realizing the segfault was a) In the Position constructor b) Specifically
    // in this method call c) It was actually above, construction from a [char]
    // wasn't precise

    // Mother Freaking Heck! There was another bug I *thought* was on this line (unrelated to the segfault)
    // and it was because C++ doesn't freaking allow constructor chaining! Who would have thought. Now, see how I found out:
    // https://stackoverflow.com/questions/187640/default-parameters-with-c-constructors
    // Yeah, that's why it has a default constructor! :) Wasted only an hour or three on this.
    put_piece(p, s);
    ++s;
  }

  #warning This returns early
  return;

  fen >> tok;
  to_move = (tok == 'w' ? White : Black);

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
      if (ThrowErrors)
        assert(0 && "Invalid Castling character in FEN.");
    }
  }

  fen >> std::skipws >> state->halfmoves >> _fullmoves;

  // TODO parse rest of FEN
}

void Position::put_piece(Piece p, Square s) {
  assert(empty(s));
  debug::print(p);

  Bitboard b = bb_from(s);
  colors_bb[p.color] |= b;
  pieces_bb[p.type] |= b;
  pieces_bb[ALL_TYPES] |= b;
  squares[s] = p;
  // Should be all?? TODO Bugtest/Perftest to catch it
}

Bitboard Position::attacks_to(Square s, Bitboard occ) const { TODO; }
Bitboard Position::sliders_to(Square s, Bitboard occ) const { TODO; }
// }}}

} // namespace Sneep
