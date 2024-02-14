#pragma once

#include "./bitboard.h"
#include "./types.h"
#include <cstdint>
#include <memory>

namespace Sneep {

class State {
  Bitboard checkers;
  Bitboard pinners[2];
  Bitboard blockers[2];

  // Squares that type of piece can move to and deliver check
  // Not used yet, but will be (final perhaps?)
  // Bitboard check_squares[6];

  Square ep;
  uint16_t ply;
  uint16_t halfmoves;

  CastleRights cr;
};

class Position {
private:
  Bitboard pieces_bb[7]; // Individual + ALL_TYPES
  Bitboard colors_bb[2];
  Piece squares[64];

  Color to_move;

  uint16_t _fullmoves;

  std::unique_ptr<State> state;

  void setup_state();
  void update_state();

  bool is_legal(Move m) const;
  bool is_legal(Move m, bool assumePL) const;
  bool is_pseudo_legal(Move m) const;

  void do_move(Move m);
  void undo_move(Move m);

  void put_piece(Piece p, Square s);
  Piece remove_piece(Square s);

  Piece moved_piece(Move m) const;

  Bitboard attacks_to(Square s) const;
  Bitboard attacks_to(Square s, Bitboard occ) const;
  Bitboard sliders_to(Square s, Bitboard occ) const;
public:
  Position();
  Position(std::string fen);

  // Note to reader: When I went back to SF to look at the operator overload to
  // print out, this is what they USED to have... now they have multiple
  // templates for this stuff. Wayyy fancier but honestly this works perfectly
  // fine for what we need... although the templates are nice for sliders
  // especially. See stockfish position.cpp, same file as in position.cpp here to cite the '<<' overload
 //% constexpr Bitboard pieces() const;
 //% constexpr Bitboard pieces(Color c) const;
 //% constexpr Bitboard pieces(PType pt) const;
 //% constexpr Bitboard pieces(Color c, PType pt) const;
 //% constexpr Bitboard pieces(PType pt1, PType pt2) const;
 //% constexpr Bitboard pieces(Color c, PType pt1, PType pt2) const;

  Bitboard pieces(PieceT pt = ALL_TYPES) const;
  Bitboard pieces(Color c) const;
  template<typename... PieceTypes> Bitboard pieces(PieceT pt, PieceTypes... pts) const;
  template<typename... PieceTypes> Bitboard pieces(Color c, PieceTypes... pts) const;


  Square king(Color c) const;
  Piece piece_on(Square s) const;
  bool empty(Square s) const;

  inline uint16_t moves() const;
};

std::ostream &operator<<(std::ostream &os, const Position &pos);

inline Piece Position::moved_piece(Move m) const {
    assert(m.is_ok());
    return squares[m.from()];
}

// Repr-related
inline Bitboard Position::pieces(PieceT pt) const {
    return pieces_bb[pt];
}
template<typename... PieceTypes>
inline Bitboard Position::pieces(PieceT pt, PieceTypes... pts) const {
    // Recursively call this function, ORing with itself
    return pieces(pt) | pieces(pts...);
}

inline Bitboard Position::pieces(Color c) const {
    return colors_bb[c];
}
template<typename... PieceTypes>
inline Bitboard Position::pieces(Color c, PieceTypes... pts) const {
    // Recursively call the above function, anded with color to only get all the pieces of the right color
    return pieces(c) & pieces(pts...);
}

inline Square Position::king(Color c) const { return lsb(pieces(c, King)); }

inline Piece Position::piece_on(Square s) const {
  assert(is_ok(s));
  return squares[s];
}
inline bool Position::empty(Square s) const { return !is_ok(piece_on(s)); }

// Attack/move-related
inline Bitboard Position::attacks_to(Square s) const {
    return attacks_to(s, pieces());
}

// Misc
inline uint16_t Position::moves() const { return _fullmoves; }
} // namespace Sneep
