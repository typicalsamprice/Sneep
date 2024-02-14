#ifndef TYPES_H_
#define TYPES_H_

#include <cassert>
#include <cstdint>

#include <bit>
#include <bitset>
#include <iostream>

namespace Sneep {
#define TODO                                                                   \
  do {                                                                         \
    assert(false && "TODO: Section unfinished");                               \
  } while (0)

// IFDEF'd garbage goes here

#if defined(THROW_MANY_ERRS)
constexpr bool ThrowExtraErrors = true;
constexpr bool ThrowErrors = true;
#else
constexpr bool ThrowExtraErrors = false;
#if defined(THROW_ERRS)
constexpr bool ThrowErrors = true;
#else
constexpr bool ThrowErrors = false;
#endif
#endif

// IFDEF'd garbage ends here

typedef uint64_t Bitboard;

enum File { File_A, File_B, File_C, File_D, File_E, File_F, File_G, File_H };
enum Rank { Rank_1, Rank_2, Rank_3, Rank_4, Rank_5, Rank_6, Rank_7, Rank_8 };

// clang-format off
// These squares use LERF encoding: https://www.chessprogramming.org/Square_Mapping_Considerations#Little-Endian_Rank-File_Mapping
enum Square {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
  NO_SQUARE
};
// clang-format on

enum Color { White, Black };

constexpr Color operator~(Color c) { return Color(c ^ Black); }

enum Direction {
  DirN = 8,
  DirS = -DirN,
  DirE = 1,
  DirW = -DirE,
  DirNE = DirN + DirE,
  DirNW = DirN + DirW,
  DirSE = DirS + DirE,
  DirSW = DirS + DirW
};

constexpr Direction operator*(int i, Direction d) {
  return Direction(int(d) * i);
}
constexpr Direction operator+(Direction d1, Direction d2) {
  return Direction(int(d1) + int(d2));
}

// Just squares + file/rank things
constexpr Square operator+(File f, Rank r) {
  return Square(int(f) + (int(r) << 3));
}

constexpr Square operator+(Square s, Direction d) {
  return Square(int(s) + int(d));
}
constexpr Square operator-(Square s, Direction d) {
  return Square(int(s) - int(d));
}
inline Square operator+=(Square &s, Direction d) { return s = s + d; }
inline Square operator-=(Square &s, Direction d) { return s = s - d; }

constexpr File file_of(Square s) { return File(int(s) & 7); }
constexpr Rank rank_of(Square s) {
  assert(s <= NO_SQUARE);
  // This is why UB happens! No other checks, it's costly :)
  return Rank(int(s) >> 3);
}

constexpr Rank back_rank(Color c) { return c == White ? Rank_1 : Rank_8; }
constexpr Rank relative_to(Rank r, Color c) {
  return c == White ? r : Rank(7 - r);
}

constexpr bool is_ok(Square s) {
  assert(s <= NO_SQUARE); // Maybe catch some UB in debug modes
  return s < NO_SQUARE;
}

constexpr Square relative_to(Square s, Color c) {
  assert(is_ok(s));
  return c == White ? s : Square(s ^ A8);
}

// Used maybe in the future by an alpha-beta search and the like.
typedef double Score;

// Pieces
enum PieceT { Pawn, Knight, Bishop, Rook, Queen, King, ALL_TYPES, NO_TYPE };
struct Piece {
public:
  Piece() : type(NO_TYPE), color(White){};
  Piece(PieceT p, Color c) : type(p), color(c){};
  Piece(char c);
  Score value() const;
  char print() const;

  PieceT type;
  Color color;
};

inline Score Piece::value() const {
  switch (type) {
  case Pawn:
    return Score(1);
  case Knight:
    return Score(3);
  case Bishop:
    return Score(3.2);
  case Rook:
    return Score(5);
  case Queen:
    return Score(9);
  case King:
    return Score(-1);
  case NO_TYPE:
  case ALL_TYPES:
    assert(false && "Piece::value called on NO_TYPE or ALL_TYPES.");
    exit(1);
  }
}
inline char Piece::print() const {
  if (type == NO_TYPE) {
    return ' ';
  }
  char rv = "pnbrqk"[type];
  if (color == White) {
    return rv - 32;
  }
  return rv;
}

inline bool is_ok(Piece p) {
  assert(p.type >= Pawn);
  return p.type < ALL_TYPES;
}

enum MoveT { Normal, EnPassant, Castle, Promotion };

// May be optimized to a uint16_t or something later.
// Bit packing sucks but I've done it before. (AKA abuse no Pawn/King promo)
class Move {
  Square _from;
  Square _to;
  MoveT _type;
  PieceT _promo;

public:
  Move(Square from, Square to) : _from(from), _to(to){};
  Move(Square from, Square to, MoveT type) : _from(from), _to(to), _type(type) {
    if (ThrowExtraErrors)
      assert(type != Promotion);
  };
  Move(Square from, Square to, MoveT type, PieceT promo)
      : _from(from), _to(to), _type(type), _promo(promo) {
    if (ThrowExtraErrors)
      assert(type == Promotion);
  };

  bool is_ok() const;
  static Move null();

  Square from() const;
  Square to() const;
  MoveT type() const;
  PieceT promo() const;
};

inline bool Move::is_ok() const {
  return ::Sneep::is_ok(_from) && ::Sneep::is_ok(_to) && _from != _to;
}
inline Move Move::null() { return Move(A1, A1); }

inline Square Move::from() const { return _from; }
inline Square Move::to() const { return _to; }
inline MoveT Move::type() const { return _type; }
inline PieceT Move::promo() const { return _promo; }

#define ENABLE_INC_OP(T)                                                       \
  constexpr T operator++(T &t) { return t = T(int(t) + 1); }                   \
  constexpr T operator--(T &t) { return t = T(int(t) - 1); }

ENABLE_INC_OP(File)
ENABLE_INC_OP(Rank)
ENABLE_INC_OP(PieceT)
ENABLE_INC_OP(Square)

#undef ENABLE_INC_OP

enum CastleRights {
  White_OO = 1,
  White_OOO = 2,
  Black_OO = 4,
  Black_OOO = 8,
  Castle_OO = White_OO | Black_OO,
  Castle_OOO = White_OOO | Black_OOO
};

} // namespace Sneep

#endif // TYPES_H_