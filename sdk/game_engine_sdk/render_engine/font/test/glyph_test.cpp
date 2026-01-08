#include "font/Glyph.h"
#include <gtest/gtest.h>

TEST(CFFDictTest, Test_PointContainment) {
    std::pair<float, float> p = {0.0f, 100.0f};
    std::vector<std::pair<float, float>> outline = {
        {0, 200}, {0, 100}, {100, 100}, {100, 200}};
    EXPECT_FALSE(font::Glyph::is_point_inside_outline(p, outline));

    p = {-10.0f, 100.0f};
    EXPECT_FALSE(font::Glyph::is_point_inside_outline(p, outline));

    p = {0.0f, 100.0f};
    outline = {{100, 200}, {200, 100}, {200, 200}};
    EXPECT_FALSE(font::Glyph::is_point_inside_outline(p, outline));

    p = {0.0f, 0.0f};
    outline = {{-10, 10}, {-10, -10}, {10, -10}, {10, 10}};
    EXPECT_TRUE(font::Glyph::is_point_inside_outline(p, outline));

    p = {0.0f, 0.0f};
    outline = {{-10, 10}, {-10, -10}, {10, -10}, {10, 0.0f}, {10, 10}};
    EXPECT_TRUE(font::Glyph::is_point_inside_outline(p, outline));
}
