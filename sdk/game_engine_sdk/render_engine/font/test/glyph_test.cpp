#include "font/winding.h"
#include <gtest/gtest.h>

TEST(CFFDictTest, Test_PointContainmentOpenSet) {
    /*
     *        x--x
     *        |  |
     *      p x--x
     */
    std::pair<float, float> p = {0.0f, 0.0f};
    std::vector<std::pair<float, float>> outline = {
        {0, 0}, {100, 0}, {100, 100}, {0, 100}};
    EXPECT_FALSE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x--x
     *        |  |
     *   p x  x--x
     */
    p = {-10.0f, 0.0f};
    outline = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
    EXPECT_FALSE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x--x
     *        |  |
     *        x--x  p x
     */
    p = {110.0f, 0.0f};
    outline = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
    EXPECT_FALSE(font::winding_number_containment_open_set(p, outline));

    /*      p x
     *        x--x
     *        |  |
     *        x--x
     */
    p = {0.0f, 110.0f};
    outline = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
    EXPECT_FALSE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x--x
     *        |  |
     *        x--x
     *      p x
     */
    p = {0.0f, -10.0f};
    outline = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
    EXPECT_FALSE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x--x
     *        \ /
     *   p x   x
     */
    p = {0.0f, 0.0f};
    outline = {{100, 100}, {200, 0}, {300, 100}};
    EXPECT_FALSE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x--x
     *        \ /
     *      p xx
     */
    p = {150.0f, 0.0f};
    outline = {{100, 100}, {200, 0}, {300, 100}};
    EXPECT_FALSE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x--x
     *        \ /
     *       p x
     */
    p = {200.0f, 0.0f};
    outline = {{100, 100}, {200, 0}, {300, 100}};
    EXPECT_FALSE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x------x
     *        |      |
     *        | p x  |
     *        |      |
     *        x------x
     */
    p = {0.0f, 0.0f};
    outline = {{-10, 10}, {-10, -10}, {10, -10}, {10, 10}};
    EXPECT_TRUE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x------x
     *        |      |
     *        | p x  x
     *        |      |
     *        x------x
     */
    p = {0.0f, 0.0f};
    outline = {{-10, 10}, {-10, -10}, {10, -10}, {12, 0.0f}, {10, 10}};
    EXPECT_TRUE(font::winding_number_containment_open_set(p, outline));

    /*
     *        x------x
     *       /        \
     *      x  p x     x
     *       \        /
     *        x------x
     */
    p = {0.0f, 0.0f};
    outline = {{-10, 10}, {-10, -10}, {10, -10}, {10, 0.0f}, {10, 10}, {-2, 5}};
    EXPECT_TRUE(font::winding_number_containment_open_set(p, outline));

    /*
     *
     *                 x - - x
     *               /        \
     *             /           \
     *           /     p x      \
     *         /                 \
     *        x - - - - - - - - - x
     */
    outline = {
        {831, 0},
        {616, 823},
        {477, 823},
        {2, 0},
    };
    p = {507, 573};
    EXPECT_TRUE(font::winding_number_containment_open_set(p, outline));
}
