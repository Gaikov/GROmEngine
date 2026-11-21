#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include "nsLib/Vec2.h"

TEST(Vec2, EqualityComparesComponentsOfDistinctObjects) {
    const nsVec2 left(2, 0.5f), right(2, 0.5f);
    EXPECT_TRUE(left == right);
    EXPECT_TRUE(right == left);
    EXPECT_FALSE(left != right);
    EXPECT_TRUE(left == left);
    EXPECT_TRUE(nsVec2() == nsVec2::null);
}

TEST(Vec2, EitherDifferentComponentMakesVectorsUnequal) {
    const nsVec2 value(2, 0.5f);
    for (const auto other : {nsVec2(3, 0.5f), nsVec2(2, 1), nsVec2(3, 1)}) {
        EXPECT_FALSE(value == other);
        EXPECT_FALSE(other == value);
        EXPECT_TRUE(value != other);
        EXPECT_TRUE(other != value);
    }
    // Equality is exact, without silently introducing an epsilon.
    EXPECT_FALSE(value == nsVec2(std::nextafter(2.0f, 3.0f), 0.5f));
}

TEST(Vec2, EqualityPreservesFloatingPointSemantics) {
    EXPECT_TRUE(nsVec2(0.0f, -0.0f) == nsVec2(-0.0f, 0.0f));
    const auto infinity = std::numeric_limits<float>::infinity();
    EXPECT_TRUE(nsVec2(infinity, -infinity) == nsVec2(infinity, -infinity));
    EXPECT_FALSE(nsVec2(infinity, 0) == nsVec2(-infinity, 0));
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    for (const auto value : {nsVec2(nan, 0), nsVec2(0, nan)}) {
        EXPECT_FALSE(value == value);
        EXPECT_TRUE(value != value);
    }
}

TEST(Vec2, ApproximateEqualityUsesScalarDefaultToleranceAndKeepsOperatorsExact) {
    const nsVec2 value(1, -2), nearby(1.00005f, -2.00005f);
    EXPECT_TRUE(value.Equal(nearby));
    EXPECT_TRUE(nearby.Equal(value));
    EXPECT_FALSE(value == nearby);
    EXPECT_TRUE(value != nearby);
    EXPECT_FALSE(value.Equal(nsVec2(1.0002f, -2)));
    EXPECT_FALSE(value.Equal(nsVec2(1, -2.0002f)));
}

TEST(Vec2, ApproximateEqualityUsesStrictComponentwiseCustomTolerance) {
    const nsVec2 origin;
    // Each component fits; the diagonal distance is greater than the tolerance.
    EXPECT_TRUE(origin.Equal(nsVec2(0.1875f, -0.1875f), 0.25f));
    EXPECT_FALSE(origin.Equal(nsVec2(0.25f, 0), 0.25f));
    EXPECT_FALSE(origin.Equal(nsVec2(0, -0.25f), 0.25f));
    EXPECT_FALSE(origin.Equal(nsVec2(0.5f, 0), 0.25f));
}

TEST(Vec2, ApproximateEqualityPreservesScalarNonFiniteAndToleranceSemantics) {
    const nsVec2 origin;
    EXPECT_TRUE(origin.Equal(origin));
    EXPECT_FALSE(origin.Equal(origin, 0));
    EXPECT_FALSE(origin.Equal(origin, -0.25f));
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    const auto infinity = std::numeric_limits<float>::infinity();
    for (const auto value : {nsVec2(nan, 0), nsVec2(0, nan),
        nsVec2(infinity, 0), nsVec2(0, -infinity)}) {
        EXPECT_FALSE(value.Equal(origin));
        EXPECT_FALSE(origin.Equal(value));
        EXPECT_FALSE(value.Equal(value));
    }
}
