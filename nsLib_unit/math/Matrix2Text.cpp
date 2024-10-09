//
// Created by Roman on 7/27/2024.
//
#include "UnitCommon.h"
#include "nsLib/math/Matrix2.h"

TEST(Matrix2, Minor) {
    nsMatrix2 m;

    float a[3][3] = {
            {10, 9, 8},
            {7, 6, 5},
            {4, 3, 1},
    };

    memcpy(m.m, a, sizeof(a));

    ASSERT_EQ((m.Minor<0, 0>()), -9.0f);
    ASSERT_EQ((m.Minor<2, 2>()), 60-9*7);
    ASSERT_EQ((m.Minor<1, 1>()), 10-4*8);

    ASSERT_EQ(m.ComputeDeterminant(), 3);
}

TEST(Matrix2, Inverse) {
    nsMatrix2 m;

/*
    float a[3][3] = {
            {10, 9, 8},
            {7, 6, 5},
            {4, 31, 11},
    };
*/

    float a[3][3] = {
            {2, 5, 7},
            {6, 3, 4},
            {5, -2, -3},
    };

    memcpy(m.m, a, sizeof(a));

    nsMatrix2 inv;
    m.Inverse(inv);
    auto m2 = m.Multiply(inv);

    ASSERT_EQ(roundf(m2._11), 1);
    ASSERT_EQ(roundf(m2._22), 1);
    ASSERT_EQ(roundf(m2._33), 1);
}

TEST(Matrix2, TransformPoint) {
    nsMatrix2 m;
    nsVec2  v(1, 1);

    m.Scale(5, 2);
    //m.Translate(4, 1);

    auto v2 = m.TransformPoint(v);

    ASSERT_EQ(v2.y, 2);
    ASSERT_EQ(v2.x, 5);
}

TEST(Matrix2, TransformVector) {
    nsMatrix2 m;
    nsVec2  v(1, 1);

    m.Transform(nsVec2(100, 100), nsVec2(2, 2), nsMath::ToRad(90));
    auto v2 = m.TransformVector(v);

    ASSERT_FLOAT_EQ(v2.y, -2);
    ASSERT_FLOAT_EQ(v2.x, 2);
}