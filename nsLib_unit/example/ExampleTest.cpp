//
// Created by Roman Gaikov on 2019-07-23.
//
#include "UnitCommon.h"

TEST(Example, EqualTrue) {
    int a = 0;
    int b = 0;
    ASSERT_EQ(a, b);
}

TEST(Example, EqualFalse) {
    int a = 0;
    int b = 1;
    ASSERT_EQ(1, b);
}


