#include "game_engine_sdk/WorldPoint.h"
#include "game_engine_sdk/equations/round.h"
#include "game_engine_sdk/physics_engine/RigidBody.h"
#include "game_engine_sdk/shape.h"
#include "logger/io.h"
#include "test_utils.h"
#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>
#include <gtest/gtest.h>

TEST(RigidBodyTest, GivenRectangleAtOrigoCreatesExpectedVertices) {
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(10.0, 10.0)};

    std::vector<glm::vec3> output = test_body.vertices();
    Round::round_mut(output[0]);
    Round::round_mut(output[1]);
    Round::round_mut(output[2]);
    Round::round_mut(output[3]);

    std::vector<glm::vec3> expected = {
        glm::vec3(-5.0, 5.0, 0),
        glm::vec3(-5.0, -5.0, 0.0),
        glm::vec3(5.0, -5.0, 0),
        glm::vec3(5.0, 5.0, 0.0),
    };

    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])
                                      << " found " << io::to_string(output[0]);
    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])
                                      << " found " << io::to_string(output[1]);
    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])
                                      << " found " << io::to_string(output[2]);
    EXPECT_EQ(expected[3], output[3]) << "Expected " << io::to_string(expected[3])
                                      << " found " << io::to_string(output[3]);
}

TEST(RigidBodyTest, GivenRectangleIsOffsetCreatesExpectedVertices) {
    RigidBody test_body = RigidBody{.position = WorldPoint(4.0, 5.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(10.0, 10.0)};

    std::vector<glm::vec3> output = test_body.vertices();
    Round::round_mut(output[0]);
    Round::round_mut(output[1]);
    Round::round_mut(output[2]);
    Round::round_mut(output[3]);

    std::vector<glm::vec3> expected = {
        glm::vec3(-1.0, 10.0, 0),
        glm::vec3(-1.0, 0.0, 0.0),
        glm::vec3(9.0, 0.0, 0),
        glm::vec3(9.0, 10.0, 0.0),
    };

    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])
                                      << " found " << io::to_string(output[0]);
    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])
                                      << " found " << io::to_string(output[1]);
    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])
                                      << " found " << io::to_string(output[2]);
    EXPECT_EQ(expected[3], output[3]) << "Expected " << io::to_string(expected[3])
                                      << " found " << io::to_string(output[3]);
}

TEST(RigidBodyTest, GivenRectangleAtOrigoAndRotatedCreatesExpectedVertices) {
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = glm::radians(-45.0),
                                    .shape = Shape::create_rectangle_data(10.0, 10.0)};

    std::vector<glm::vec3> output = test_body.vertices();
    Round::round_mut(output[0]);
    Round::round_mut(output[1]);
    Round::round_mut(output[2]);
    Round::round_mut(output[3]);

    std::vector<glm::vec3> expected = {
        glm::vec3(-7.071, 0.0, 0),
        glm::vec3(0.0, -7.071, 0.0),
        glm::vec3(7.071, 0.0, 0),
        glm::vec3(0.0, 7.071, 0.0),
    };

    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])
                                      << " found " << io::to_string(output[0]);
    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])
                                      << " found " << io::to_string(output[1]);
    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])
                                      << " found " << io::to_string(output[2]);
    EXPECT_EQ(expected[3], output[3]) << "Expected " << io::to_string(expected[3])
                                      << " found " << io::to_string(output[3]);
}

TEST(RigidBodyTest, GivenRectangleIsOffsetAndRotatedCreatesExpectedVertices) {
    RigidBody test_body = RigidBody{.position = WorldPoint(1.0, 2.0, 0.0),
                                    .rotation = glm::radians(-45.0),
                                    .shape = Shape::create_rectangle_data(10.0, 10.0)};

    std::vector<glm::vec3> output = test_body.vertices();
    Round::round_mut(output[0]);
    Round::round_mut(output[1]);
    Round::round_mut(output[2]);
    Round::round_mut(output[3]);

    std::vector<glm::vec3> expected = {
        glm::vec3(-6.071, 2.0, 0),
        glm::vec3(1.0, -5.071, 0.0),
        glm::vec3(8.071, 2.0, 0),
        glm::vec3(1.0, 9.071, 0.0),
    };

    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])
                                      << " found " << io::to_string(output[0]);
    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])
                                      << " found " << io::to_string(output[1]);
    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])
                                      << " found " << io::to_string(output[2]);
    EXPECT_EQ(expected[3], output[3]) << "Expected " << io::to_string(expected[3])
                                      << " found " << io::to_string(output[3]);
}

TEST(RigidBodyTest, GivenTriangleAtOrigoCreatesExpectedVertices) {
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(1.0)};

    std::vector<glm::vec3> output = test_body.vertices();
    Round::round_mut(output[0]);
    Round::round_mut(output[1]);
    Round::round_mut(output[2]);

    std::vector<glm::vec3> expected = {glm::vec3(0.0, 0.577, 0),
                                       glm::vec3(-0.5, -0.289, 0.0),
                                       glm::vec3(0.5, -0.289, 0)};
    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])
                                      << " found " << io::to_string(output[0]);
    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])
                                      << " found " << io::to_string(output[1]);
    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])
                                      << " found " << io::to_string(output[2]);
}

/*TEST(RigidBodyTest, GivenTriangleAtOrigoCreatesExpectedVertices2) {*/
/*    ShapeData test_body_shape =*/
/*        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 400.0}}};*/
/*    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),*/
/*                                    .rotation = 0.0,*/
/*                                    .shape = test_body_shape};*/
/**/
/*    std::vector<glm::vec3> output = test_body.vertices();*/
/*    Round::round_mut(output[0]);*/
/*    Round::round_mut(output[1]);*/
/*    Round::round_mut(output[2]);*/
/**/
/*    std::vector<glm::vec3> expected = {*/
/*        glm::vec3(0, 230.94, 0), glm::vec3(-200, -115.47, 0), glm::vec3(200, -115.47,
 * 0)};*/
/*    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])*/
/*                                      << " found " << io::to_string(output[0]);*/
/*    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])*/
/*                                      << " found " << io::to_string(output[1]);*/
/*    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])*/
/*                                      << " found " << io::to_string(output[2]);*/
/*}*/

TEST(RigidBodyTest, GivenTriangleOffsetCreatesExpectedVertices) {
    RigidBody test_body = RigidBody{.position = WorldPoint(-10.0, 15.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(1.0)};

    std::vector<glm::vec3> output = test_body.vertices();
    Round::round_mut(output[0]);
    Round::round_mut(output[1]);
    Round::round_mut(output[2]);

    std::vector<glm::vec3> expected = {glm::vec3(-10.0, 15.577, 0),
                                       glm::vec3(-10.5, 14.711, 0.0),
                                       glm::vec3(-9.5, 14.711, 0)};
    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])
                                      << " found " << io::to_string(output[0]);
    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])
                                      << " found " << io::to_string(output[1]);
    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])
                                      << " found " << io::to_string(output[2]);
}

TEST(RigidBodyTest, GivenTriangleAtOrigoAndRotatedCreatesExpectedVertices) {
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = -degrees_30,
                                    .shape = Shape::create_triangle_data(1.0)};

    std::vector<glm::vec3> output = test_body.vertices();
    Round::round_mut(output[0]);
    Round::round_mut(output[1]);
    Round::round_mut(output[2]);

    std::vector<glm::vec3> expected = {glm::vec3(-0.289, 0.500, 0.0),
                                       glm::vec3(-0.289, -0.500, 0.0),
                                       glm::vec3(0.577, 0.0, 0.0)};
    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])
                                      << " found " << io::to_string(output[0]);
    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])
                                      << " found " << io::to_string(output[1]);
    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])
                                      << " found " << io::to_string(output[2]);
}

TEST(RigidBodyTest, GivenTriangleIsOffsetAndRotatedCreatesExpectedVertices) {
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = WorldPoint(15.0, -10.0, 0.0),
                                    .rotation = -degrees_30,
                                    .shape = Shape::create_triangle_data(1.0)};

    std::vector<glm::vec3> output = test_body.vertices();
    Round::round_mut(output[0]);
    Round::round_mut(output[1]);
    Round::round_mut(output[2]);

    std::vector<glm::vec3> expected = {glm::vec3(14.711, -9.500, 0.0),
                                       glm::vec3(14.711, -10.500, 0.0),
                                       glm::vec3(15.577, -10.0, 0.0)};
    EXPECT_EQ(expected[0], output[0]) << "Expected " << io::to_string(expected[0])
                                      << " found " << io::to_string(output[0]);
    EXPECT_EQ(expected[1], output[1]) << "Expected " << io::to_string(expected[1])
                                      << " found " << io::to_string(output[1]);
    EXPECT_EQ(expected[2], output[2]) << "Expected " << io::to_string(expected[2])
                                      << " found " << io::to_string(output[2]);
}

TEST(RigidBodyTest, GivenPointAtOrigoWhenRectangleIsAtOrigoExpectTrue) {
    WorldPoint test_point = WorldPoint(0.0, 0.0, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(1.0, 1.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest, GivenPointOnRectangleBorderWhenRectangleIsAtOrigoExpectTrue) {
    WorldPoint test_point = WorldPoint(0.5, 0.0, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(1.0, 1.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest, GivenPointOnRectangleBorderWhenRectangleIsOffsetExpectTrue) {
    WorldPoint test_point = WorldPoint(-10.5, 15.0, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(-10.0, 15.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(1.0, 1.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest,
     GivenPointOnRectangleBorderWhenRectangleIsOffsetAndRotatedExpectTrue) {
    float degrees_45 = glm::pi<float>() / 4.0;
    WorldPoint test_point = WorldPoint(-10.707, 15.0, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(-10.0, 15.0, 0.0),
                                    .rotation = -degrees_45,
                                    .shape = Shape::create_rectangle_data(1.0, 1.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest,
     GivenPointOutsideRectangleBorderWhenRectangleIsOffsetAndRotatedExpectFalse) {
    float degrees_45 = glm::pi<float>() / 4.0;
    WorldPoint test_point = WorldPoint(-10.708, 15.0, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(-10.0, 15.0, 0.0),
                                    .rotation = -degrees_45,
                                    .shape = Shape::create_rectangle_data(1.0, 1.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_FALSE(output);
}

TEST(RigidBodyTest, GivenPointOutsideRectangleBorderWhenRectangleIsOffsetExpectFalse) {
    WorldPoint test_point = WorldPoint(-10.501, 15.0, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(-10.0, 15.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(1.0, 1.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_FALSE(output);
}

TEST(RigidBodyTest, GivenPointOutsideRectangleBorderWhenRectangleIsAtOrigoExpectFalse) {
    WorldPoint test_point = WorldPoint(-0.501, 0.0, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(1.0, 1.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_FALSE(output);
}

TEST(RigidBodyTest, GivenTriangleAtOrigoWhenPointsOnTriangleBorder) {
    WorldPoint on_center = WorldPoint(0.0, 0.0, 0.0);
    WorldPoint on_corner_top = WorldPoint(0.0, 0.577, 0.0);
    WorldPoint off_corner_top = WorldPoint(0.0, 0.578, 0.0);
    WorldPoint off_corner_top_left = WorldPoint(-0.01, 0.577, 0.0);
    WorldPoint off_corner_top_right = WorldPoint(0.01, 0.577, 0.0);

    WorldPoint on_corner_left = WorldPoint(-0.499, -0.288, 0);
    WorldPoint on_corner_right = WorldPoint(0.499, -0.288, 0);
    WorldPoint off_bottom = WorldPoint(0.0, -0.289, 0);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(1.0)};
    bool output_on_center = test_body.is_point_inside(on_center);

    bool output_on_corner_top = test_body.is_point_inside(on_corner_top);
    bool output_off_corner_top = test_body.is_point_inside(off_corner_top);
    bool output_off_corner_top_left = test_body.is_point_inside(off_corner_top_left);
    bool output_off_corner_top_right = test_body.is_point_inside(off_corner_top_right);

    bool output_on_corner_left = test_body.is_point_inside(on_corner_left);
    bool output_on_corner_right = test_body.is_point_inside(on_corner_right);
    bool output_off_bottom = test_body.is_point_inside(off_bottom);

    EXPECT_TRUE(output_on_center);
    EXPECT_TRUE(output_on_corner_top);
    EXPECT_FALSE(output_off_corner_top);
    EXPECT_FALSE(output_off_corner_top_left);
    EXPECT_FALSE(output_off_corner_top_right);
    EXPECT_TRUE(output_on_corner_left);
    EXPECT_TRUE(output_on_corner_right);
    EXPECT_FALSE(output_off_bottom);
}

TEST(RigidBodyTest, GivenTriangleIsOffsetWhenPointsOnTriangleBorder) {
    WorldPoint on_top_corner = WorldPoint(400.0, 630.94, 0.0);
    WorldPoint off_top_corner = WorldPoint(400.0, 630.95, 0.0);
    WorldPoint off_top_corner_left = WorldPoint(399.999, 630.94, 0.0);
    WorldPoint off_top_corner_right = WorldPoint(400.001, 630.94, 0.0);

    WorldPoint on_left_corner = WorldPoint(200.01, 284.531, 0.0);
    WorldPoint on_right_corner = WorldPoint(599.99, 284.531, 0.0);

    WorldPoint off_bottom = WorldPoint(400.0, 284.529, 0.0);

    RigidBody test_body = RigidBody{.position = WorldPoint(400.0, 400.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(400.0)};
    bool output_on_top_corner = test_body.is_point_inside(on_top_corner);
    bool output_off_top_corner = test_body.is_point_inside(off_top_corner);
    bool output_off_top_corner_left = test_body.is_point_inside(off_top_corner_left);
    bool output_off_top_corner_right = test_body.is_point_inside(off_top_corner_right);
    bool output_on_left_corner = test_body.is_point_inside(on_left_corner);
    bool output_on_right_corner = test_body.is_point_inside(on_right_corner);
    bool output_off_bottom = test_body.is_point_inside(off_bottom);

    EXPECT_TRUE(output_on_top_corner);
    EXPECT_FALSE(output_off_top_corner);
    EXPECT_FALSE(output_off_top_corner_left);
    EXPECT_FALSE(output_off_top_corner_right);
    EXPECT_TRUE(output_on_left_corner);
    EXPECT_TRUE(output_on_right_corner);
    EXPECT_FALSE(output_off_bottom);
}

TEST(RigidBodyTest, GivenPointOnTriangleBorderWhenTriangleIsAtOrigoAndScaledExpectTrue) {
    WorldPoint test_point = WorldPoint(0.0, 1.154, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(2.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest, GivenPointOutsideTriangleWhenTriangleIsAtOrigoAndScaledExpectFalse) {
    WorldPoint test_point = WorldPoint(0.0, 1.555, 0.0);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(2.0)};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_FALSE(output);
}

TEST(RigidBodyTest, GivenPointOnTriangleBorderWhenTriangleIsAtOrigoAndRotatedExpectTrue) {
    WorldPoint off_right_corner = WorldPoint(-0.289, 0.2, 0.0);
    WorldPoint on_left_border = WorldPoint(-0.288, 0.0, 0.0);
    WorldPoint off_left_border = WorldPoint(-0.289, 0.0, 0.0);
    float degrees_30 = glm::radians(-30.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = degrees_30,
                                    .shape = Shape::create_triangle_data(1.0)};
    bool output_on_left_border = test_body.is_point_inside(on_left_border);
    bool output_off_left_border = test_body.is_point_inside(off_left_border);
    bool output_off_right_corner = test_body.is_point_inside(off_right_corner);

    EXPECT_TRUE(output_on_left_border);
    EXPECT_FALSE(output_off_left_border);
    EXPECT_FALSE(output_off_right_corner);
}

TEST(RigidBodyTest, GivenRectangleWhenAtOrigoProducesExpectedNormals) {
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(10.0, 10.0)};
    std::vector<glm::vec3> normals = test_body.normals();
    Round::round_mut(normals[0]);
    Round::round_mut(normals[1]);
    Round::round_mut(normals[2]);
    Round::round_mut(normals[3]);

    std::vector<glm::vec3> expected = {
        glm::vec3(-1.0, 0.0, 0.0),
        glm::vec3(0.0, -1.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
    };
    EXPECT_EQ(expected[0], normals[0])
        << "Expected " << expected[0] << " found " << normals[0];
    EXPECT_EQ(expected[1], normals[1])
        << "Expected " << expected[1] << " found " << normals[1];
    EXPECT_EQ(expected[2], normals[2])
        << "Expected " << expected[2] << " found " << normals[2];
    EXPECT_EQ(expected[3], normals[3])
        << "Expected " << expected[3] << " found " << normals[3];

    auto magnitude0 = Round::round(glm::length(expected[0]));
    auto magnitude1 = Round::round(glm::length(expected[1]));
    auto magnitude2 = Round::round(glm::length(expected[2]));
    auto magnitude3 = Round::round(glm::length(expected[3]));
    EXPECT_EQ(magnitude0, 1.0) << "Expected length 1.0 found " << magnitude0 << std::endl;
    EXPECT_EQ(magnitude1, 1.0) << "Expected length 1.0 found " << magnitude1 << std::endl;
    EXPECT_EQ(magnitude2, 1.0) << "Expected length 1.0 found " << magnitude2 << std::endl;
    EXPECT_EQ(magnitude3, 1.0) << "Expected length 1.0 found " << magnitude3 << std::endl;
}

TEST(RigidBodyTest, GivenRectangleWhenOffsetProducesExpectedNormals) {
    RigidBody test_body = RigidBody{.position = WorldPoint(10.0, 7.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(10.0, 10.0)};
    std::vector<glm::vec3> normals = test_body.normals();
    Round::round_mut(normals[0]);
    Round::round_mut(normals[1]);
    Round::round_mut(normals[2]);
    Round::round_mut(normals[3]);

    std::vector<glm::vec3> expected = {
        glm::vec3(-1.0, 0.0, 0.0),
        glm::vec3(0.0, -1.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
    };
    EXPECT_EQ(expected[0], normals[0]) << "Expected " << io::to_string(expected[0])
                                       << " found " << io::to_string(normals[0]);
    EXPECT_EQ(expected[1], normals[1]) << "Expected " << io::to_string(expected[1])
                                       << " found " << io::to_string(normals[1]);
    EXPECT_EQ(expected[2], normals[2]) << "Expected " << io::to_string(expected[2])
                                       << " found " << io::to_string(normals[2]);
    EXPECT_EQ(expected[3], normals[3]) << "Expected " << io::to_string(expected[3])
                                       << " found " << io::to_string(normals[3]);

    auto magnitude0 = Round::round(glm::length(expected[0]));
    auto magnitude1 = Round::round(glm::length(expected[1]));
    auto magnitude2 = Round::round(glm::length(expected[2]));
    auto magnitude3 = Round::round(glm::length(expected[3]));
    EXPECT_EQ(magnitude0, 1.0) << "Expected length 1.0 found " << magnitude0 << std::endl;
    EXPECT_EQ(magnitude1, 1.0) << "Expected length 1.0 found " << magnitude1 << std::endl;
    EXPECT_EQ(magnitude2, 1.0) << "Expected length 1.0 found " << magnitude2 << std::endl;
    EXPECT_EQ(magnitude3, 1.0) << "Expected length 1.0 found " << magnitude3 << std::endl;
}

TEST(RigidBodyTest, GivenRectangleWhenIsOffsetAndRotatedProducesExpectedNormals) {
    RigidBody test_body = RigidBody{.position = WorldPoint(7.0, -6.0, 0.0),
                                    .rotation = glm::radians(-45.0),
                                    .shape = Shape::create_rectangle_data(10.0, 10.0)};
    std::vector<glm::vec3> normals = test_body.normals();
    Round::round_mut(normals[0]);
    Round::round_mut(normals[1]);
    Round::round_mut(normals[2]);
    Round::round_mut(normals[3]);

    std::vector<glm::vec3> expected = {
        glm::vec3(-0.707, -0.707, 0.0),
        glm::vec3(0.707, -0.707, 0.0),
        glm::vec3(0.707, 0.707, 0.0),
        glm::vec3(-0.707, 0.707, 0.0),
    };
    EXPECT_EQ(expected[0], normals[0]) << "Expected " << io::to_string(expected[0])
                                       << " found " << io::to_string(normals[0]);
    EXPECT_EQ(expected[1], normals[1]) << "Expected " << io::to_string(expected[1])
                                       << " found " << io::to_string(normals[1]);
    EXPECT_EQ(expected[2], normals[2]) << "Expected " << io::to_string(expected[2])
                                       << " found " << io::to_string(normals[2]);
    EXPECT_EQ(expected[3], normals[3]) << "Expected " << io::to_string(expected[3])
                                       << " found " << io::to_string(normals[3]);

    auto magnitude0 = Round::round(glm::length(expected[0]));
    auto magnitude1 = Round::round(glm::length(expected[1]));
    auto magnitude2 = Round::round(glm::length(expected[2]));
    auto magnitude3 = Round::round(glm::length(expected[3]));
    EXPECT_EQ(magnitude0, 1.0) << "Expected length 1.0 found " << magnitude0 << std::endl;
    EXPECT_EQ(magnitude1, 1.0) << "Expected length 1.0 found " << magnitude1 << std::endl;
    EXPECT_EQ(magnitude2, 1.0) << "Expected length 1.0 found " << magnitude2 << std::endl;
    EXPECT_EQ(magnitude3, 1.0) << "Expected length 1.0 found " << magnitude3 << std::endl;
}

TEST(RigidBodyTest, GivenTriangleWhenAtOrigoProducesExpectedNormals) {
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(1.0)};
    std::vector<glm::vec3> normals_ = test_body.normals();
    std::vector<glm::vec3> normals = {
        Round::round(normals_[0]), Round::round(normals_[1]), Round::round(normals_[2])};

    std::vector<glm::vec3> expected = {
        glm::vec3(-0.866, 0.5, 0), glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.866, 0.5, 0)};
    EXPECT_EQ(expected[0], normals[0])
        << "Expected " << expected[0] << " found " << normals[0];
    EXPECT_EQ(expected[1], normals[1])
        << "Expected " << expected[1] << " found " << normals[1];
    EXPECT_EQ(expected[2], normals[2])
        << "Expected " << expected[2] << " found " << normals[2];

    auto magnitude0 = Round::round(glm::length(expected[0]));
    auto magnitude1 = Round::round(glm::length(expected[1]));
    auto magnitude2 = Round::round(glm::length(expected[2]));
    EXPECT_EQ(magnitude0, 1.0) << "Expected length 1.0 found " << magnitude0 << std::endl;
    EXPECT_EQ(magnitude1, 1.0) << "Expected length 1.0 found " << magnitude1 << std::endl;
    EXPECT_EQ(magnitude2, 1.0) << "Expected length 1.0 found " << magnitude2 << std::endl;
}

TEST(RigidBodyTest, GivenTriangleWhenAtOrigoAndRotatedProducesExpectedNormals) {
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = -degrees_30,
                                    .shape = Shape::create_triangle_data(1.0)};
    std::vector<glm::vec3> normals_ = test_body.normals();
    std::vector<glm::vec3> normals = {
        Round::round(normals_[0]), Round::round(normals_[1]), Round::round(normals_[2])};

    std::vector<glm::vec3> expected = {
        glm::vec3(-1.0, 0.0, 0),
        glm::vec3(0.5, -0.866, 0),
        glm::vec3(0.5, 0.866, 0.0),
    };
    EXPECT_EQ(expected[0], normals[0]) << "Expected " << io::to_string(expected[0])
                                       << " found " << io::to_string(normals[0]);
    EXPECT_EQ(expected[1], normals[1]) << "Expected " << io::to_string(expected[1])
                                       << " found " << io::to_string(normals[1]);
    EXPECT_EQ(expected[2], normals[2]) << "Expected " << io::to_string(expected[2])
                                       << " found " << io::to_string(normals[2]);

    auto magnitude0 = Round::round(glm::length(expected[0]));
    auto magnitude1 = Round::round(glm::length(expected[1]));
    auto magnitude2 = Round::round(glm::length(expected[2]));
    EXPECT_EQ(magnitude0, 1.0) << "Expected length 1.0 found " << magnitude0 << std::endl;
    EXPECT_EQ(magnitude1, 1.0) << "Expected length 1.0 found " << magnitude1 << std::endl;
    EXPECT_EQ(magnitude2, 1.0) << "Expected length 1.0 found " << magnitude2 << std::endl;
}

TEST(RigidBodyTest, GivenTriangleWhenIsOffsetAndRotatedProducesExpectedNormals) {
    float degrees_30 = glm::radians(-30.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(-15.0, 10.0, 0.0),
                                    .rotation = degrees_30,
                                    .shape = Shape::create_triangle_data(1.0)};
    std::vector<glm::vec3> normals_ = test_body.normals();
    std::vector<glm::vec3> normals = {
        Round::round(normals_[0]), Round::round(normals_[1]), Round::round(normals_[2])};

    std::vector<glm::vec3> expected = {
        glm::vec3(-1.0, 0.0, 0),
        glm::vec3(0.5, -0.866, 0),
        glm::vec3(0.5, 0.866, 0.0),
    };
    EXPECT_EQ(expected[0], normals[0]) << "Expected " << io::to_string(expected[0])
                                       << " found " << io::to_string(normals[0]);
    EXPECT_EQ(expected[1], normals[1]) << "Expected " << io::to_string(expected[1])
                                       << " found " << io::to_string(normals[1]);
    EXPECT_EQ(expected[2], normals[2]) << "Expected " << io::to_string(expected[2])
                                       << " found " << io::to_string(normals[2]);

    auto magnitude0 = Round::round(glm::length(expected[0]));
    auto magnitude1 = Round::round(glm::length(expected[1]));
    auto magnitude2 = Round::round(glm::length(expected[2]));
    EXPECT_EQ(magnitude0, 1.0) << "Expected length 1.0 found " << magnitude0 << std::endl;
    EXPECT_EQ(magnitude1, 1.0) << "Expected length 1.0 found " << magnitude1 << std::endl;
    EXPECT_EQ(magnitude2, 1.0) << "Expected length 1.0 found " << magnitude2 << std::endl;
}

TEST(RigidBodyTest, GivenRectangleWhenAtOrigoProducesExpectedEdges) {
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(1.0, 1.0)};

    auto edges = test_body.edges();
    std::vector<glm::vec3> expected = {
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
    };

    EXPECT_EQ(expected[0], edges[0])
        << "Expected " << expected[0] << " found " << edges[0];
    EXPECT_EQ(expected[1], edges[1])
        << "Expected " << expected[1] << " found " << edges[1];
    EXPECT_EQ(expected[2], edges[2])
        << "Expected " << expected[2] << " found " << edges[2];
    EXPECT_EQ(expected[3], edges[3])
        << "Expected " << expected[3] << " found " << edges[3];
}

TEST(RigidBodyTest, GivenTriangleWhenAtOrigoProducesExpectedEdges) {
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(1.0)};

    auto edges = test_body.edges();
    std::vector<glm::vec3> expected = {
        glm::vec3(-0.5f, -0.866025f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(-0.5f, 0.866025f, 0.0f),
    };

    float max_diff = 1e-3f;
    expect_near(expected[0], edges[0], max_diff);
    expect_near(expected[1], edges[1], max_diff);
    expect_near(expected[2], edges[2], max_diff);
}

TEST(RigidBodyTest, GivenPointLeftOfRectangleExpectClosestPointOnBorder) {
    WorldPoint test_point = WorldPoint(-15.0f, 0.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = 0.0f,
                                    .shape = Shape::create_rectangle_data(10.0f, 10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    WorldPoint expected = WorldPoint(-5.0f, 0.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointLeftOfRotated90RectangleExpectClosestPointOnBorder) {
    WorldPoint test_point = WorldPoint(-15.0f, 0.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = glm::radians(-90.0f),
                                    .shape = Shape::create_rectangle_data(10.0f, 10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(-5.0f, 0.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointAboveRectangleExpectClosestPointOnBorder) {
    WorldPoint test_point = WorldPoint(0.0f, 15.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = 0.0f,
                                    .shape = Shape::create_rectangle_data(10.0f, 10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    WorldPoint expected = WorldPoint(0.0f, 5.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointAboveRotated90RectangleExpectClosestPointOnBorder) {
    WorldPoint test_point = WorldPoint(0.0f, 15.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = glm::radians(-90.f),
                                    .shape = Shape::create_rectangle_data(10.0f, 10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(0.0f, 5.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointDiagonalToRectangleExpectClosestPointOnCorner) {
    WorldPoint test_point = WorldPoint(-15.0f, 15.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = 0.0f,
                                    .shape = Shape::create_rectangle_data(10.0f, 10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    WorldPoint expected = WorldPoint(-5.0f, 5.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointDiagonalToRotatedNeg90RectangleExpectClosestPointOnCorner) {
    WorldPoint test_point = WorldPoint(-15.0f, 15.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = glm::radians(90.f),
                                    .shape = Shape::create_rectangle_data(10.0f, 10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(-5.0f, 5.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointDiagonalToRotated45RectangleExpectClosestPointOnDiagonal) {
    WorldPoint test_point = WorldPoint(-30.0f, 30.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = glm::radians(45.0f), // Negated from -PI/4
                                    .shape = Shape::create_rectangle_data(20.0f, 20.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(-7.071f, 7.071f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest,
     GivenPointOnXAxisWithRectRotated90AndOffsetYExpectClosestPointOnEdge) {
    WorldPoint test_point = WorldPoint(-400.0f, 0.0f, 0.0f);
    RigidBody test_body =
        RigidBody{.position = WorldPoint(0.0f, -150.0f, 0.0f),
                  .rotation = glm::radians(-90.0f), // Negated from PI/2
                  .shape = Shape::create_rectangle_data(500.0f, 500.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(-250.0f, 0.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest,
     GivenPointOnYAxisWithRectRotated90AndOffsetYExpectClosestPointOnEdge) {
    WorldPoint test_point = WorldPoint(0.0f, 400.0f, 0.0f);
    RigidBody test_body =
        RigidBody{.position = WorldPoint(0.0f, -150.0f, 0.0f),
                  .rotation = glm::radians(90.0f), // Negated from -PI/2
                  .shape = Shape::create_rectangle_data(500.0f, 500.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(0.0f, 100.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest,
     GivenPointOnXAxisWithRectRotated90AndOffsetXExpectClosestPointOnEdge) {
    WorldPoint test_point = WorldPoint(-400.0f, 0.0f, 0.0f);
    RigidBody test_body =
        RigidBody{.position = WorldPoint(150.0f, 0.0f, 0.0f),
                  .rotation = glm::radians(90.0f), // Negated from -PI/2
                  .shape = Shape::create_rectangle_data(500.0f, 500.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(-100.0f, 0.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest,
     GivenPointOnYAxisWithRectRotated90AndOffsetXExpectClosestPointOnEdge) {
    WorldPoint test_point = WorldPoint(0.0f, 400.0f, 0.0f);
    RigidBody test_body =
        RigidBody{.position = WorldPoint(150.0f, 0.0f, 0.0f),
                  .rotation = glm::radians(90.0f), // Negated from -PI/2
                  .shape = Shape::create_rectangle_data(500.0f, 500.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(0.0f, 250.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest,
     GivenAsyncOffsetPointWithRectRotated90AndAsyncOffsetExpectClosestPointOnEdge) {
    WorldPoint test_point = WorldPoint(70.0f, -25.0f, 0.0f);
    RigidBody test_body =
        RigidBody{.position = WorldPoint(-150.0f, 50.0f, 0.0f),
                  .rotation = glm::radians(-90.0f), // Negated from PI/2
                  .shape = Shape::create_rectangle_data(200.0f, 200.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(-50.0f, -25.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointIsInsideRectangle) {
    WorldPoint test_point = WorldPoint(0.0f, 0.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(10.0f, 10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    Round::round_mut(output);
    WorldPoint expected = WorldPoint(0.0f, 0.0f, 0.0f);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointIsAboveTriangleExpectPointOnVertex) {
    WorldPoint test_point = WorldPoint(0.0f, 15.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    WorldPoint expected = WorldPoint(0.0f, 5.774f, 0.0f);
    Round::round_mut(output);
    Round::round_mut(expected);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointIsBelowTriangleExpectPointOnVertex) {
    WorldPoint test_point = WorldPoint(0.0f, -15.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    WorldPoint expected = WorldPoint(0.0f, -2.887f, 0.0f);
    Round::round_mut(output);
    Round::round_mut(expected);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointIsLeftOfTriangleAndTriangleIsRotated30ExpectPointOnVertex) {
    WorldPoint test_point = WorldPoint(15.0f, 0.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0f, 0.0f, 0.0f),
                                    .rotation = glm::radians(30.0f),
                                    .shape = Shape::create_triangle_data(10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    WorldPoint expected = WorldPoint(2.887f, 0.0f, 0.0f);
    Round::round_mut(output);
    Round::round_mut(expected);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest, GivenPointIsBelowOfTriangleAndTriangleIsOffsetExpectPointOnVertex) {
    WorldPoint test_point = WorldPoint(10.0f, 50.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(10.0f, 100.0f, 0.0f),
                                    .rotation = 0.0f,
                                    .shape = Shape::create_triangle_data(10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    WorldPoint expected = WorldPoint(10.0f, 97.113f, 0.0f);
    Round::round_mut(output);
    Round::round_mut(expected);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}

TEST(RigidBodyTest,
     GivenPointIsLeftOfTriangleAndTriangleIsOffsetAndRotatedExpectPointOnVertex) {
    WorldPoint test_point = WorldPoint(0.0f, 100.0f, 0.0f);
    RigidBody test_body = RigidBody{.position = WorldPoint(10.0f, 100.0f, 0.0f),
                                    .rotation = glm::radians(90.0f),
                                    .shape = Shape::create_triangle_data(10.0f)};
    WorldPoint output = test_body.closest_point_on_body(test_point);
    WorldPoint expected = WorldPoint(7.113f, 100.0f, 0.0f);
    Round::round_mut(output);
    Round::round_mut(expected);
    EXPECT_EQ(expected, output) << "Expected " << expected << " found " << output;
}
