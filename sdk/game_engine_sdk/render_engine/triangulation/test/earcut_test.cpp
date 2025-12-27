#include "triangulation/earcut.h"
#include <gtest/gtest.h>

TEST(TriangulationEarClipTests, Test_EarClipTriangle) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::pair<int, int>> polygon = {{0, 0}, {10, 0}, {5, 5}};

    const auto triangles = triangulation::earclip(polygon);

    EXPECT_EQ(1, triangles.size());

    EXPECT_EQ(triangles[0][0], std::pair(5, 5));
    EXPECT_EQ(triangles[0][1], std::pair(0, 0));
    EXPECT_EQ(triangles[0][2], std::pair(10, 0));
}

TEST(TriangulationEarClipTests, Test_EarClipQuad) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::pair<int, int>> polygon = {
        {-5, -5},
        {5, -5},
        {5, 5},
        {-5, 5},
    };

    const auto triangles = triangulation::earclip(polygon);

    EXPECT_EQ(2, triangles.size());

    EXPECT_EQ(triangles[0][0], std::pair(-5, 5));
    EXPECT_EQ(triangles[0][1], std::pair(-5, -5));
    EXPECT_EQ(triangles[0][2], std::pair(5, -5));

    EXPECT_EQ(triangles[1][0], std::pair(-5, 5));
    EXPECT_EQ(triangles[1][1], std::pair(5, -5));
    EXPECT_EQ(triangles[1][2], std::pair(5, 5));
}

TEST(TriangulationEarClipTests, Test_EarClipSimplePolygon) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::pair<int, int>> polygon = {
        {3, 48},   {52, 8},    {99, 50}, {138, 25}, {175, 77},
        {131, 72}, {111, 113}, {72, 43}, {26, 55},  {29, 100}};

    const auto triangles = triangulation::earclip(polygon);

    EXPECT_EQ(8, triangles.size());

    EXPECT_EQ(triangles[0][0], std::pair(99, 50));
    EXPECT_EQ(triangles[0][1], std::pair(138, 25));
    EXPECT_EQ(triangles[0][2], std::pair(175, 77));

    EXPECT_EQ(triangles[1][0], std::pair(99, 50));
    EXPECT_EQ(triangles[1][1], std::pair(175, 77));
    EXPECT_EQ(triangles[1][2], std::pair(131, 72));

    EXPECT_EQ(triangles[2][0], std::pair(99, 50));
    EXPECT_EQ(triangles[2][1], std::pair(131, 72));
    EXPECT_EQ(triangles[2][2], std::pair(111, 113));

    EXPECT_EQ(triangles[3][0], std::pair(99, 50));
    EXPECT_EQ(triangles[3][1], std::pair(111, 113));
    EXPECT_EQ(triangles[3][2], std::pair(72, 43));

    EXPECT_EQ(triangles[4][0], std::pair(26, 55));
    EXPECT_EQ(triangles[4][1], std::pair(29, 100));
    EXPECT_EQ(triangles[4][2], std::pair(3, 48));

    EXPECT_EQ(triangles[5][0], std::pair(26, 55));
    EXPECT_EQ(triangles[5][1], std::pair(3, 48));
    EXPECT_EQ(triangles[5][2], std::pair(52, 8));

    EXPECT_EQ(triangles[6][0], std::pair(52, 8));
    EXPECT_EQ(triangles[6][1], std::pair(99, 50));
    EXPECT_EQ(triangles[6][2], std::pair(72, 43));

    EXPECT_EQ(triangles[7][0], std::pair(52, 8));
    EXPECT_EQ(triangles[7][1], std::pair(72, 43));
    EXPECT_EQ(triangles[7][2], std::pair(26, 55));
}
