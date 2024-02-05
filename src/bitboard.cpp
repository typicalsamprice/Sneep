#include <string>

#include "./types.h"
#include "./bitboard.h"

namespace Sneep {

void pretty_print(Bitboard b) {
    using std::cout, std::endl;
    std::string sep("+---+---+---+---+---+---+---+---+\n");
    cout << sep;
    // TODO Add decrement ops
    for (Rank r = Rank_Eight; r >= Rank_One; r = Rank(int(r) - 1)) {
        for (File f = File_A; f <= File_H; f = File(int(f) + 1)) {
            Bitboard bit = bb_from(f + r) & b;
            cout << "| " << (bit ? "x" : " ") << " ";
        }
        cout << "|\n" << sep;
    }

    cout << endl;
}

} // namespace Sneep
