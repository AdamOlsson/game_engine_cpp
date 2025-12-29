#include "math/interpolate.h"
#include <gtest/gtest.h>

TEST(LerpTests, Test_LERPWithIntegers) {
    const std::pair<int, int> a = {0, 0};
    const std::pair<int, int> b = {10, 0};
    const std::pair<int, int> c = math::lerp(a, b, 0.5f);
    EXPECT_EQ(c, std::pair(5, 0));

    const std::pair<int, int> d = {0, 0};
    const std::pair<int, int> e = {0, 10};
    const std::pair<int, int> f = math::lerp(d, e, 0.5f);
    EXPECT_EQ(f, std::pair(0, 5));

    const std::pair<int, int> g = {2, 2};
    const std::pair<int, int> h = {10, 10};
    const std::pair<int, int> i = math::lerp(g, h, 0.5f);
    EXPECT_EQ(i, std::pair(6, 6));
}
