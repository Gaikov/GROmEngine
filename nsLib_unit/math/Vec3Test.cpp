#include <gtest/gtest.h>

#include <limits>
#include "nsLib/Vec3.h"

TEST(Vec3, FiniteIncludesZeroNegativeAndExtremeFiniteComponents) {
    EXPECT_TRUE(nsVec3().IsFinite());
    const nsVec3 ordinary(1, -2, 3);
    EXPECT_TRUE(ordinary.IsFinite());
    const auto largest = std::numeric_limits<float>::max();
    const auto smallest = std::numeric_limits<float>::denorm_min();
    const nsVec3 extremes(largest, -largest, smallest);
    EXPECT_TRUE(extremes.IsFinite());
}

TEST(Vec3, RejectsNanAndEitherInfinityInEveryComponent) {
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    const auto infinity = std::numeric_limits<float>::infinity();
    for (const auto value : {nan, infinity, -infinity}) {
        EXPECT_FALSE(nsVec3(value, 2, 3).IsFinite());
        EXPECT_FALSE(nsVec3(1, value, 3).IsFinite());
        EXPECT_FALSE(nsVec3(1, 2, value).IsFinite());
    }
}
