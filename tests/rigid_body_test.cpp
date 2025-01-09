#include "glm/ext/scalar_constants.hpp"
#include "glm/glm.hpp"
#include "physics_engine/RigidBody.h"
#include "shape.h"
#include <gtest/gtest.h>

TEST(RigidBodyTest, GivenPointAtOrigoWhenTriangleIsAtOrigoExpectTrue) {
    glm::vec2 test_point = glm::vec2(0.0, 0.0);
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    RigidBody test_body = RigidBody{.position = glm::vec3(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest, GivenPointOnTriangleBorderWhenTriangleIsAtOrigoExpectTrue) {
    glm::vec2 test_point = glm::vec2(0.0, 0.577); // Top of triangle
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    RigidBody test_body = RigidBody{.position = glm::vec3(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest, GivenPointOutsideTriangleWhenTriangleIsAtOrigoExpectFalse) {
    glm::vec2 test_point = glm::vec2(0.0, 0.578); // Top of triangle
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    RigidBody test_body = RigidBody{.position = glm::vec3(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_FALSE(output);
}

TEST(RigidBodyTest, GivenPointOnTriangleBorderWhenTriangleIsAtOrigoAndScaledExpectTrue) {
    glm::vec2 test_point = glm::vec2(0.0, 1.154); // Top of triangle
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 2.0}}};
    RigidBody test_body = RigidBody{.position = glm::vec3(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest, GivenPointOutsideTriangleWhenTriangleIsAtOrigoAndScaledExpectFalse) {
    glm::vec2 test_point = glm::vec2(0.0, 1.555); // Top of triangle
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 2.0}}};
    RigidBody test_body = RigidBody{.position = glm::vec3(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_FALSE(output);
}

TEST(RigidBodyTest, GivenPointOnTriangleBorderWhenTriangleIsAtOrigoAndRotatedExpectTrue) {
    glm::vec2 test_point = glm::vec2(-0.288, 0.2);
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = glm::vec3(0.0, 0.0, 0.0),
                                    .rotation = degrees_30,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest, GivenPointOutsideTriangleWhenTriangleIsAtOrigoAndRotatedExpectFalse) {
    glm::vec2 test_point = glm::vec2(-0.289, 0.2);
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = glm::vec3(0.0, 0.0, 0.0),
                                    .rotation = degrees_30,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_FALSE(output);
}

TEST(RigidBodyTest, GivenPointOnTriangleBorderWhenTriangleIsOffsetAndRotatedExpectTrue) {
    glm::vec2 test_point = glm::vec2(-10.288, 10.2);
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = glm::vec3(-10.0, 10.0, 0.0),
                                    .rotation = degrees_30,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_TRUE(output);
}

TEST(RigidBodyTest, GivenPointOutsideTriangleWhenTriangleIsOffsetAndRotatedExpectFalse) {
    glm::vec2 test_point = glm::vec2(-10.289, 10.2);
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = glm::vec3(-10.0, 10.0, 0.0),
                                    .rotation = degrees_30,
                                    .shape_data = test_body_shape};
    bool output = test_body.is_point_inside(test_point);
    EXPECT_FALSE(output);
}
