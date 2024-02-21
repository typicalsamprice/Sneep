#include "types.h"

namespace Sneep {

Piece::Piece(char c) {
  if (c > 'Z') {
    color = Black;
  } else {
    c += 32;
    color = White;
  }


  switch (c) {
  case 'p':
    type = Pawn;
    break;
  case 'n':
    type = Knight;
    break;
  case 'b':
    type = Bishop;
    break;
  case 'r':
    type = Rook;
    break;
  case 'q':
    type = Queen;
    break;
  case 'k':
    type = King;
    break;
  default:
    if (ThrowErrors) {
      assert(0 && "Invalid Piece character");
    }
  }
}

std::ostream &operator<<(std::ostream &os, const Piece &pc) {
  os << "Piece { type: " << pc.type << ", color: " << pc.color << " }";
  return os;
}

} // namespace Sneep
