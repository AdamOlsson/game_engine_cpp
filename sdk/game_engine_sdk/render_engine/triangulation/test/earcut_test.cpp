#include "triangulation/mapbox/earcut.h"
#include <gtest/gtest.h>

TEST(TriangulationEarCutTests, Test_EarCutTriangle) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::vector<std::pair<int, int>>> polygon = {
        {{0, 0}, {10, 0}, {5, 5}}};

    const auto indices = mapbox::earcut(polygon);

    EXPECT_EQ(3, indices.size());
}

TEST(TriangulationEarCutTests, Test_EarCutQuad) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::vector<std::pair<int, int>>> polygon = {
        {{-5, -5}, {5, -5}, {5, 5}, {-5, 5}}};

    const auto indices = mapbox::earcut(polygon);

    EXPECT_EQ(6, indices.size());
}

TEST(TriangulationEarCutTests, Test_EarCutSimplePolygon) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::vector<std::pair<int, int>>> polygon = {{{3, 48},
                                                                    {52, 8},
                                                                    {99, 50},
                                                                    {138, 25},
                                                                    {175, 77},
                                                                    {131, 72},
                                                                    {111, 113},
                                                                    {72, 43},
                                                                    {26, 55},
                                                                    {29, 100}}};

    const auto indices = mapbox::earcut(polygon);

    EXPECT_EQ(24, indices.size());
}

TEST(TriangulationEarCutTests, Test_EarCutTriangleWithHole) {
    // Note the counter clockwise ordering, positive y up, holes clockwise ordering
    const std::vector<std::pair<int, int>> exterior = {{0, 0}, {10, 0}, {5, 5}};
    const std::vector<std::pair<int, int>> hole = {{2, 1}, {5, 4}, {8, 1}};
    const std::vector<std::vector<std::pair<int, int>>> polygon = {exterior, hole};
    const auto indices = mapbox::earcut(polygon);

    EXPECT_EQ(18 - 2 + 2, indices.size());
}

TEST(TriangulationEarCutTests, Test_EarCutQuadWithHole) {
    // Note the counter clockwise ordering, positive y up, holes clockwise ordering
    const std::vector<std::pair<int, int>> exterior = {
        {-5, -5}, {5, -5}, {5, 5}, {-5, 5}};
    const std::vector<std::pair<int, int>> hole = {{{4, -4}, {-4, -4}, {-4, 4}, {4, 4}}};

    const std::vector<std::vector<std::pair<int, int>>> polygon = {exterior, hole};
    const auto indices = mapbox::earcut(polygon);

    EXPECT_EQ(24 - 2 + 2, indices.size());
}
