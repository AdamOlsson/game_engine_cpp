#include "triangulation/earcut.h"
#include <gtest/gtest.h>

TEST(TriangulationEarCutTests, Test_EarCutTriangle) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::pair<int, int>> polygon = {{0, 0}, {10, 0}, {5, 5}};

    const auto triangles = triangulation::Earcut<int>::run(polygon, {});

    EXPECT_EQ(1, triangles.indices.size());

    EXPECT_EQ(triangles.indices[0][0], 2);
    EXPECT_EQ(triangles.indices[0][1], 0);
    EXPECT_EQ(triangles.indices[0][2], 1);
}

TEST(TriangulationEarCutTests, Test_EarCutTriangleWithHole) {
    // Note the counter clockwise ordering, positive y up, holes clockwise ordering
    const std::vector<std::pair<int, int>> polygon = {{0, 0}, {10, 0}, {5, 5}};
    const std::vector<std::vector<std::pair<int, int>>> hole = {{{2, 1}, {5, 4}, {8, 1}}};

    const auto triangles = triangulation::Earcut<int>::run(polygon, hole);

    // Expected number of triangles is number of vertices - 2, the number of vertices
    // is 3 for exterior and 3 for hole but the bridging adds 2 as well
    EXPECT_EQ(6 - 2 + 2, triangles.indices.size());
}

TEST(TriangulationEarCutTests, Test_EarCutQuad) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::pair<int, int>> polygon = {{-5, -5}, {5, -5}, {5, 5}, {-5, 5}};

    const auto triangles = triangulation::Earcut<int>::run(polygon, {});

    EXPECT_EQ(2, triangles.indices.size());

    EXPECT_EQ(triangles.indices[0][0], 3);
    EXPECT_EQ(triangles.indices[0][1], 0);
    EXPECT_EQ(triangles.indices[0][2], 1);

    EXPECT_EQ(triangles.indices[1][0], 3);
    EXPECT_EQ(triangles.indices[1][1], 1);
    EXPECT_EQ(triangles.indices[1][2], 2);
}

TEST(TriangulationEarCutTests, Test_EarCutQuadWithHole) {
    // Note the counter clockwise ordering, positive y up, holes clockwise ordering
    const std::vector<std::pair<int, int>> polygon = {{-5, -5}, {5, -5}, {5, 5}, {-5, 5}};
    const std::vector<std::vector<std::pair<int, int>>> holes = {
        {{4, -4}, {-4, -4}, {-4, 4}, {4, 4}}};

    const auto triangles = triangulation::Earcut<int>::run(polygon, holes);

    // Expected number of triangles is number of vertices - 2, the number of vertices
    // is 4 for exterior and 4 for hole but the bridging adds 2 as well
    EXPECT_EQ(8 - 2 + 2, triangles.indices.size());
}

TEST(TriangulationEarCutTests, Test_EarCutSimplePolygon) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::pair<int, int>> polygon = {
        {3, 48},   {52, 8},    {99, 50}, {138, 25}, {175, 77},
        {131, 72}, {111, 113}, {72, 43}, {26, 55},  {29, 100}};

    const auto triangles = triangulation::Earcut<int>::run(polygon, {});

    EXPECT_EQ(8, triangles.indices.size());

    EXPECT_EQ(triangles.indices[0][0], 2);
    EXPECT_EQ(triangles.indices[0][1], 3);
    EXPECT_EQ(triangles.indices[0][2], 4);

    EXPECT_EQ(triangles.indices[1][0], 2);
    EXPECT_EQ(triangles.indices[1][1], 4);
    EXPECT_EQ(triangles.indices[1][2], 5);

    EXPECT_EQ(triangles.indices[2][0], 2);
    EXPECT_EQ(triangles.indices[2][1], 5);
    EXPECT_EQ(triangles.indices[2][2], 6);

    EXPECT_EQ(triangles.indices[3][0], 2);
    EXPECT_EQ(triangles.indices[3][1], 6);
    EXPECT_EQ(triangles.indices[3][2], 7);

    EXPECT_EQ(triangles.indices[4][0], 8);
    EXPECT_EQ(triangles.indices[4][1], 9);
    EXPECT_EQ(triangles.indices[4][2], 0);

    EXPECT_EQ(triangles.indices[5][0], 8);
    EXPECT_EQ(triangles.indices[5][1], 0);
    EXPECT_EQ(triangles.indices[5][2], 1);

    EXPECT_EQ(triangles.indices[6][0], 1);
    EXPECT_EQ(triangles.indices[6][1], 2);
    EXPECT_EQ(triangles.indices[6][2], 7);

    EXPECT_EQ(triangles.indices[7][0], 1);
    EXPECT_EQ(triangles.indices[7][1], 7);
    EXPECT_EQ(triangles.indices[7][2], 8);
}

TEST(TriangulationEarCutTests, Test_IsConvex) {
    EXPECT_FALSE(triangulation::detail::EarcutImpl<float>::is_convex(
        std::pair<float, float>(0, 5), std::pair<float, float>(9, 5),
        std::pair<float, float>(7, 4)));
}

TEST(TriangulationEarCutTests, Test_EarCutSimplePolygonMinimumAngleReflexVertex) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::pair<float, float>> external_outline = {
        {0, 5}, {9, 5}, {7, 4}, {8, 4}, {10, 0}, {10, 8}, {0, 8}};
    const std::vector<std::vector<std::pair<float, float>>> internal_outlines = {
        {{1, 6}, {1, 7}, {2, 6}}};

    const std::vector<std::pair<float, float>> expected_bridged_vertices = {
        {0, 5}, {9, 5}, {2, 6}, {1, 6},  {1, 7},  {2, 6},
        {9, 5}, {7, 4}, {8, 4}, {10, 0}, {10, 8}, {0, 8}};

    const auto bridged_vertices =
        triangulation::detail::EarcutImpl<float>::bridge_exterior_and_interior(
            external_outline, internal_outlines);

    EXPECT_EQ(expected_bridged_vertices.size(), bridged_vertices.size());
    for (size_t i = 0; i < bridged_vertices.size(); i++) {
        EXPECT_EQ(expected_bridged_vertices[i], bridged_vertices[i]);
    }
}

TEST(TriangulationEarCutTests, Test_EarCutSimplePolygonWith2HolesExample1) {
    // Note the counter clockwise ordering, positive y up
    const std::vector<std::pair<float, float>> external_outline = {
        {0, 0}, {4, 0}, {3.2, 8}, {0, 8}};
    const std::vector<std::vector<std::pair<float, float>>> internal_outlines_one_hole = {
        {{1.5, 3}, {1.5, 4}, {2.5, 3.5}}};

    const std::vector<std::pair<float, float>> expected_bridged_vertices_one_hole = {
        {0, 0},     {4, 0}, {2.5, 3.5}, {1.5, 3}, {1.5, 4},
        {2.5, 3.5}, {4, 0}, {3.2, 8},   {0, 8}};

    const auto bridged_vertices_one_hole =
        triangulation::detail::EarcutImpl<float>::bridge_exterior_and_interior(
            external_outline, internal_outlines_one_hole);

    EXPECT_EQ(expected_bridged_vertices_one_hole.size(),
              bridged_vertices_one_hole.size());
    for (size_t i = 0; i < expected_bridged_vertices_one_hole.size(); i++) {
        EXPECT_EQ(bridged_vertices_one_hole[i], expected_bridged_vertices_one_hole[i]);
    }

    const std::vector<std::vector<std::pair<float, float>>> internal_outlines_two_holes =
        {
            {{1, 1}, {1, 2}, {2, 1.5}},
            {{1.5, 3}, {1.5, 4}, {2.5, 3.5}},
        };

    const std::vector<std::pair<float, float>> expected_bridged_vertices_two_holes = {
        {0, 0},     {4, 0},   {2, 1.5}, {1, 1},     {1, 2}, {2, 1.5}, {4, 0},
        {2.5, 3.5}, {1.5, 3}, {1.5, 4}, {2.5, 3.5}, {4, 0}, {3.2, 8}, {0, 8}};

    const auto bridged_vertices_two_holes =
        triangulation::detail::EarcutImpl<float>::bridge_exterior_and_interior(
            external_outline, internal_outlines_two_holes);
    EXPECT_EQ(expected_bridged_vertices_two_holes.size(),
              bridged_vertices_two_holes.size());
    for (size_t i = 0; i < expected_bridged_vertices_two_holes.size(); i++) {
        EXPECT_EQ(bridged_vertices_two_holes[i], expected_bridged_vertices_two_holes[i]);
    }

    const triangulation::Triangles triangles =
        triangulation::Earcut<float>::run(external_outline, internal_outlines_two_holes);
    const auto vertices = triangles.vertices;
    const std::vector<triangulation::Triangle> indices = triangles.indices;

    EXPECT_EQ(12, triangles.indices.size());
}

TEST(TriangulationEarCutTests, Test_EarCutSimplePolygonWith2HolesExample2) {
    const std::vector<std::pair<float, float>> external_outline = {
        {0, 0}, {4, 0}, {3, 10}, {0, 10}};
    const std::vector<std::vector<std::pair<float, float>>> internal_outlines_one_hole = {
        {{1.5, 1}, {1.5, 2}, {2.5, 1.5}}};

    const std::vector<std::pair<float, float>> expected_bridged_vertices_one_hole = {
        {0, 0},     {4, 0}, {2.5, 1.5}, {1.5, 1}, {1.5, 2},
        {2.5, 1.5}, {4, 0}, {3, 10},    {0, 10}};

    const auto bridged_vertices_one_hole =
        triangulation::detail::EarcutImpl<float>::bridge_exterior_and_interior(
            external_outline, internal_outlines_one_hole);

    EXPECT_EQ(expected_bridged_vertices_one_hole.size(),
              bridged_vertices_one_hole.size());
    for (size_t i = 0; i < expected_bridged_vertices_one_hole.size(); i++) {
        EXPECT_EQ(bridged_vertices_one_hole[i], expected_bridged_vertices_one_hole[i]);
    }

    const std::vector<std::vector<std::pair<float, float>>> internal_outlines_two_holes =
        {
            {{1.5, 1}, {1.5, 2}, {2.5, 1.5}},
            {{1, 3}, {1, 4}, {2, 3.5}},
        };

    const std::vector<std::pair<float, float>> expected_bridged_vertices_two_holes = {
        {0, 0},   {4, 0}, {2.5, 1.5}, {1.5, 1}, {1.5, 2}, {2.5, 1.5}, {4, 0},
        {2, 3.5}, {1, 3}, {1, 4},     {2, 3.5}, {4, 0},   {3, 10},    {0, 10}};

    const auto bridged_vertices_two_holes =
        triangulation::detail::EarcutImpl<float>::bridge_exterior_and_interior(
            external_outline, internal_outlines_two_holes);
    EXPECT_EQ(expected_bridged_vertices_two_holes.size(),
              bridged_vertices_two_holes.size());
    for (size_t i = 0; i < expected_bridged_vertices_two_holes.size(); i++) {
        EXPECT_EQ(bridged_vertices_two_holes[i], expected_bridged_vertices_two_holes[i]);
    }

    const triangulation::Triangles triangles =
        triangulation::Earcut<float>::run(external_outline, internal_outlines_two_holes);
    const auto vertices = triangles.vertices;
    const std::vector<triangulation::Triangle> indices = triangles.indices;

    EXPECT_EQ(12, triangles.indices.size());
}
