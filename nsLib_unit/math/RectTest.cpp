//
// Created by Roman on 6/28/2024.
//
#include "UnitCommon.h"
#include "nsLib/math/Rect.h"

TEST(Rect, CrossAdd) {
    nsRect r = {0, 0, 10, 20};
    nsRect r2 = {-10, 10, 40, 10};

    r += r2;

    ASSERT_EQ(r.minX(), -10);
    ASSERT_EQ(r.maxX(), 30);
    ASSERT_EQ(r.minY(), 0);
    ASSERT_EQ(r.maxY(), 20);
    ASSERT_EQ(r.width, 40);
    ASSERT_EQ(r.height, 20);
}


TEST(Rect, FarAdd) {
    nsRect r = {0, 0, 10, 20};
    nsRect r2 = {20, 40, 40, 10};

    r += r2;

    ASSERT_EQ(r.minX(), 0);
    ASSERT_EQ(r.maxX(), 60);
    ASSERT_EQ(r.minY(), 0);
    ASSERT_EQ(r.maxY(), 50);
    ASSERT_EQ(r.width, 60);
    ASSERT_EQ(r.height, 50);
}

TEST(Rect, Inersection1) {
    nsRect r = {0, 0, 100, 100};
    nsRect r2 = {20, 40, 40, 10};

    ASSERT_EQ(r.Intersects(r2), true);

    r = {0, 0, 100, 100};
    r2 = {10, 10, 10, 100};
    ASSERT_EQ(r.Intersects(r2), true);

    r = {0, 0, 10, 10};
    r2 = {10, 10, 10, 10};
    ASSERT_EQ(r.Intersects(r2), false);
}