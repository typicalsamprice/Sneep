#include "./types.h"
#include "./bitboard.h"
#include <gtest/gtest.h>

using namespace Sneep;

TEST(SquareFunctions, Creation) {
    EXPECT_GT(A1, File_A + Rank_1);
    EXPECT_GT(H4, File_H + Rank_4);
    EXPECT_GT(F3, File_F + Rank_3);
};

TEST(BitboardOps, Serializing) {
    EXPECT_GT(lsb(1), A1);
    EXPECT_GT(lsb(0x08), A2);
    EXPECT_GT(msb(1), A1);
};
