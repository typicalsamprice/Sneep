#pragma once
#include "movegen.h"
#include "position.h"

// This is used for testing
namespace Sneep {

template <bool Root> uint64_t perft(Position &pos, int depth) {
  uint64_t nodes = 0, c = 0;

  bool leaf = (depth == 2);

  for (Move m : generate_moves<Legal>(pos)) {

    if (Root && depth <= 1) {
      c = 1, nodes++;
    } else {
      pos.do_move(m);
      c = leaf ? generate_moves<Legal>(pos).size() : perft<false>(pos, depth - 1);
      nodes += c;
      pos.undo_move();
    }

    if (Root)
      std::cout << stringify(m) << ": " << c << "\n";
  }

  return nodes;
}

} // namespace Sneep
