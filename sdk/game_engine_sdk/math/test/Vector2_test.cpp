
#include "math/Vector2.h"
#include <gtest/gtest.h>

TEST(Vector2Tests, Test_AxisReference) {
    math::Vector2 vec = math::Vector2(0.0f, 0.0f);
    EXPECT_EQ(vec.x, 0.0f);
    EXPECT_EQ(vec.y, 0.0f);

    vec.x = 1.0f;
    vec.y = 1.0f;

    EXPECT_EQ(vec.x, 1.0f);
    EXPECT_EQ(vec.y, 1.0f);

    vec.rotate_z(-45.0f);

    EXPECT_NEAR(vec.x, 1.41421354f, 1e-6);
    EXPECT_EQ(vec.y, 0.0f);
}
