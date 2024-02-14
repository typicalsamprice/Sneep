#include <string>

#include "./types.h"
#include "./bitboard.h"

#include <iostream>

namespace Sneep {

void initialize_bitboards() {
    TODO;
}

void pretty_print(const Bitboard b) {
    std::string sep("+---+---+---+---+---+---+---+---+\n");
    std::cout << sep;
    for (Rank r = Rank_8; r >= Rank_1; --r) {
        for (File f = File_A; f <= File_H; ++f) {
            Bitboard bit = bb_from(f + r) & b;
            std::cout << "| " << (bit ? "x" : " ") << " ";
        }
        std::cout << "|\n" << sep;
    }
}

} // namespace Sneep
