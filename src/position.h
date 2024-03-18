#pragma once

#include "./bitboard.h"
#include "./types.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace Sneep {

class State {
public:
  Bitboard checkers;
  Bitboard pinners[2];
  Bitboard blockers[2];

  // Squares that type of piece can move to and deliver check
  // Not used yet, but will be (final perhaps?)
  // Bitboard check_squares[6];

  Square ep;
  uint16_t halfmoves;

  CastleRights cr;
  std::optional<Piece> captured;

  State* prev;

  bool is_ok() const;
};

inline bool State::is_ok() const {
  if (halfmoves > 75) // Shouldn't happen...we draw before then
    return false;

  if (popcnt(checkers) > 2) // How even
    return false;

  if (QuickStateCheck)
    goto AfterPinnerBlockerCheck;

  if (popcnt(blockers[White]) > 8)
    return false;
  if (popcnt(blockers[Black]) > 8)
    return false;
  if (popcnt(pinners[White]) > 8)
    return false;
  if (popcnt(pinners[Black]) > 8)
    return false;

AfterPinnerBlockerCheck:

  return true;
}

class Position {
private:
  Bitboard pieces_bb[7]; // Individual + ALL_TYPES
  Bitboard colors_bb[2];
  Piece squares[64];

  Color side_to_move;

  uint16_t _fullmoves;

  State* state;

  std::vector<Move> hist;

  void setup_state();
  void update_state();
    // Not `const` since we pass a reference to part of State,
    // which is is part of ourself.
  Bitboard calc_pinners_blockers(Bitboard &pinners, const Color us);

  void put_piece(Piece p, Square s);
  Piece remove_piece(Square s);
  template<bool Do> void do_castle(const CastleRights castle);

public:
  Color to_move() const { return side_to_move; };
  Square ep() const { return state->ep; }
  CastleRights castle_rights() const { return state->cr; }

  Square king_of(const Color col) const { return lsb(pieces(col, King)); };

  // Cannot just assign a string to a Position and expect it to work out.
  explicit Position(
      std::string fen =
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  void do_move(const Move m);
  void undo_move(); // Automatically undoes the last Move.

  bool is_legal(const Move m, const bool checkPL) const;
  bool is_pseudo_legal(Move m) const;

  Piece moved_piece(Move m) const;
  std::optional<Piece> captured_piece(Move m) const;

  Bitboard attacks_to(Square s) const;
  Bitboard attacks_to(Square s, Bitboard occ) const;
  Bitboard sliders_to(Square s, Bitboard occ) const;

  // Note to reader: When I went back to SF to look at the operator overload to
  // print out, this is what they USED to have... now they have multiple
  // templates for this stuff. Wayyy fancier but honestly the old way works
  // perfectly fine for what we need... although the templates are nice for
  // sliders especially. See stockfish position.cpp, same file as in
  // position.cpp here to cite the '<<' overload
  //% constexpr Bitboard pieces() const;
  //% constexpr Bitboard pieces(Color c) const;
  //% constexpr Bitboard pieces(PType pt) const;
  //% constexpr Bitboard pieces(Color c, PType pt) const;
  //% constexpr Bitboard pieces(PType pt1, PType pt2) const;
  //% constexpr Bitboard pieces(Color c, PType pt1, PType pt2) const;

  Bitboard pieces(PieceT pt = ALL_TYPES) const;
  Bitboard pieces(Color c) const;
  template <typename... PieceTypes>
  Bitboard pieces(PieceT pt, PieceTypes... pts) const;
  template <typename... PieceTypes>
  Bitboard pieces(Color c, PieceTypes... pts) const;

  Square king(Color c) const;
  Piece piece_on(Square s) const;
  bool empty(Square s) const;

  inline uint16_t moves() const;
};

// The aforementioned operator overload I stole from SF.
std::ostream &operator<<(std::ostream &os, const Position &pos);

inline Piece Position::moved_piece(Move m) const {
  assert(m.is_ok());
  return squares[m.from()];
}
inline std::optional<Piece> Position::captured_piece(Move m) const {
  // This is an `optional` to furthur encourage CHECKING if there is a valid
  // captured piece not for any other good reason. If they were more ergonomic
  // in C++, I would 100% use something like Rust's Option<T> or Error<T, E>
  // wayyy more often.
  Square capture_square = m.type() == EnPassant ? (file_of(m.to()) + rank_of(m.from())) : m.to();
  assert(m.is_ok());
  if (empty(capture_square))
    return std::nullopt;
  return std::optional<Piece>{piece_on(capture_square)};
}

// Repr-related
inline Bitboard Position::pieces(PieceT pt) const {
  //assert(popcnt(pieces_bb[King]) == 2);
  return pieces_bb[pt];
}
template <typename... PieceTypes>
inline Bitboard Position::pieces(PieceT pt, PieceTypes... pts) const {
  // Recursively call this function, ORing with itself
  return pieces(pt) | pieces(pts...);
}

inline Bitboard Position::pieces(Color c) const { return colors_bb[c]; }
template <typename... PieceTypes>
inline Bitboard Position::pieces(Color c, PieceTypes... pts) const {
  // Recursively call the above function, anded with color to only get all the
  // pieces of the right color
  return pieces(c) & pieces(pts...);
}

inline Square Position::king(Color c) const {
  assert(pieces(c, King)); // Can't be nonzero.... I dearly hope
  return lsb(pieces(c, King));
}

inline Piece Position::piece_on(Square s) const {
  assert(is_ok(s));
  return squares[s];
}
inline Piece Position::remove_piece(Square s) {
  Piece p = piece_on(s);
  Bitboard bs = bb_from(s);

  if (!empty(s)) {
    squares[s] = Piece();
    pieces_bb[p.type] ^= bs;
    pieces_bb[ALL_TYPES] ^= bs;
    colors_bb[p.color] ^= bs;
  }

  return p;
}
inline bool Position::empty(Square s) const {
  return piece_on(s).type == NO_TYPE;
}

// Attack/move-related
inline Bitboard Position::attacks_to(Square s) const {
  return attacks_to(s, pieces());
}

// Misc
inline uint16_t Position::moves() const { return _fullmoves; }
} // namespace Sneep
