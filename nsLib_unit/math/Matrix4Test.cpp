#include <gtest/gtest.h>

#include "nsLib/matrix4.h"

TEST(Matrix4, ChainedProductsHaveIndependentResults) {
    nsMatrix a, b, c;
    a.Scale(2, 3, 4);
    b.Identity();
    b.SetPos(nsVec3(1, 2, 3));
    c.Scale(5, 7, 11);

    const auto &first = a * b;
    const auto combined = (a * b) * c;
    EXPECT_FLOAT_EQ(combined._11, 10);
    EXPECT_FLOAT_EQ(combined._22, 21);
    EXPECT_FLOAT_EQ(combined._33, 44);
    EXPECT_FLOAT_EQ(combined._41, 5);
    EXPECT_FLOAT_EQ(combined._42, 14);
    EXPECT_FLOAT_EQ(combined._43, 33);
    EXPECT_FLOAT_EQ(first._11, 2);
    EXPECT_FLOAT_EQ(first._41, 1);
}

TEST(Matrix4, InvertsViewProjectionInPlace) {
    nsMatrix view, projection;
    view.SetLookAtRH(nsVec3(3, 2, 5), nsVec3(0, 0, 0), nsVec3(0, 1, 0));
    projection.SetProjFovRH(1.1f, 1.5f, 0.1f, 100.0f);
    const nsMatrix original = view * projection;
    auto inverse = original;
    ASSERT_TRUE(inverse.Inverse(inverse));
    const nsMatrix product = original * inverse;
    for (auto row = 0; row < 4; ++row) {
        for (auto column = 0; column < 4; ++column)
            EXPECT_NEAR(product.m[row][column], row == column ? 1.0f : 0.0f, 5e-5f);
    }
}

TEST(Matrix4, HandlesSmallScalesAndPreservesResultOnFailure) {
    nsMatrix scale;
    scale.Scale(1e-20f, 2, 3);
    nsMatrix inverse;
    ASSERT_TRUE(scale.Inverse(inverse));
    EXPECT_FLOAT_EQ(inverse._11, 1e20f);

    scale.Scale(0, 2, 3);
    EXPECT_FALSE(scale.Inverse(inverse));
    EXPECT_FLOAT_EQ(inverse._11, 1e20f);
}
