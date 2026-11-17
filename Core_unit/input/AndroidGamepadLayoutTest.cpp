#include "Platform/AndroidPlatform/AndroidGamepadLayout.h"
#include <gtest/gtest.h>

TEST(AndroidGamepadLayoutTest, KeysProduceCanonicalDirections) {
    constexpr uint8_t expected[] = {1, 2, 4, 8, 5, 6, 9, 10};
    for (auto key = 0; key < 8; ++key) {
        EXPECT_EQ(nsAndroidGamepadLayout::DpadDirections(1 << key), expected[key]);
    }
    EXPECT_EQ(nsAndroidGamepadLayout::DpadDirections(0), 0);
}

TEST(AndroidGamepadLayoutTest, ReleasingDiagonalDoesNotReleaseAnIndependentHeldKey) {
    auto keys = uint8_t((1 << 0) | (1 << 4)); // Up + UpLeft.
    EXPECT_EQ(nsAndroidGamepadLayout::DpadDirections(keys), 5);
    keys &= ~(1 << 4);
    EXPECT_EQ(nsAndroidGamepadLayout::DpadDirections(keys), 1);
    keys = uint8_t((1 << 4) | (1 << 6)); // UpLeft + UpRight.
    EXPECT_EQ(nsAndroidGamepadLayout::DpadDirections(keys), 13);
    keys &= ~(1 << 4);
    EXPECT_EQ(nsAndroidGamepadLayout::DpadDirections(keys), 9);
}
