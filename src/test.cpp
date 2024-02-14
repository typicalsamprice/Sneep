#include <iostream>
#include <cassert>
#include "./types.h"
#include "./bitboard.h"

int main() {
    using namespace std;
    using namespace Sneep;

    cout << "Testing square functions:" << endl;
    assert(relative_to(A1, Black) == A8);
    assert(relative_to(A2, Black) == A7);

    assert(rank_of(A3) == Rank_3);
    assert(rank_of(G5) == Rank_5);
    assert(file_of(H6) == File_H);
    assert(file_of(B1) == File_B);

    assert((File_H + Rank_3) == H3);

    assert(bb_from(A1) == 1);

}
