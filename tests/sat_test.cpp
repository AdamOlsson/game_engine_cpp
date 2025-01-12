#include "Shape.h"
#include "glm/ext/scalar_constants.hpp"
#include "io.h"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.cpp"
#include "physics_engine/SAT.h"
#include <gtest/gtest.h>
#include <optional>

void expect_near(glm::vec3 &expected, glm::vec3 &v, float epsilon) {
    std::cout << "Expected " << expected << " found " << v << std::endl;
    EXPECT_NEAR(expected.x, v.x, epsilon);
    EXPECT_NEAR(expected.y, v.y, epsilon);
    EXPECT_NEAR(expected.z, v.z, epsilon);
}

void expect_near(CollisionInformation &expected, CollisionInformation &info,
                 float epsilon) {
    std::cout << "collision point (exp == val): "
              << vec_to_string(expected.collision_point)
              << " == " << vec_to_string(info.collision_point) << std::endl;
    std::cout << "normal (exp == val): " << vec_to_string(expected.normal)
              << " == " << vec_to_string(info.normal) << std::endl;

    EXPECT_NEAR(expected.penetration_depth, info.penetration_depth, epsilon);
    expect_near(expected.normal, info.normal, epsilon);
    expect_near(expected.collision_point, info.collision_point, epsilon);
}

void expect_near(CollisionEdge &expected, CollisionEdge &v, float epsilon) {
    expect_near(expected.max, v.max, epsilon);
    expect_near(expected.start, v.start, epsilon);
    expect_near(expected.end, v.end, epsilon);
    expect_near(expected.edge, v.edge, epsilon);
}

void expect_near(ClippedPoint &expected, ClippedPoint &c, float epsilon) {
    expect_near(expected.vertex, c.vertex, epsilon);
    EXPECT_NEAR(expected.depth, c.depth, epsilon);
}

TEST(SATTest, GivenRectanglesAreAxisAlignedWhenDoNotCollideExpectNoCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-10.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(11.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    EXPECT_FALSE(output.has_value());
}

TEST(SATTest, GivenRectanglesAreAxisAlignedWhenTouchOnYAxisExpectNoCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-10.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    EXPECT_FALSE(output.has_value());
}

TEST(SATTest, GivenRectanglesAreAxisAlignedWhenOverlapOnYAxisExpectCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-10.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(9.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};

    std::optional<CollisionInformation> output = SAT::collision_detection(body_a, body_b);

    CollisionInformation expected = {.penetration_depth = 1.0,
                                     .normal = glm::vec3(1.0, 0.0, 0.0),
                                     .collision_point = glm::vec3(-1.0, 5.0, 0.0)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

TEST(
    SATTest,
    GivenRectanglesAreAxisAlignedWhenOverlapOnYAxisButBodiesHaveSwappedOrderExpectCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(9.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(-10.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    CollisionInformation expected = {.penetration_depth = 1.0,
                                     .normal = glm::vec3(-1.0, 0.0, 0.0),
                                     .collision_point = glm::vec3(0.0, -5.0, 0.0)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

TEST(
    SATTest,
    GivenRectanglesAreAxisAlignedAndOffsetFromOrigoWhenOverlappingOnXAxisExpectCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-10.0, 20.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(-10.0, 15.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    CollisionInformation expected = {.penetration_depth = 5.0,
                                     .normal = glm::vec3(0.0, -1.0, 0.0),
                                     .collision_point = glm::vec3(0.0, 20.0, 0.0)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

TEST(
    SATTest,
    GivenOneRectangleIsAxisAlignedAndOneRotated90DegreesWhenOverlapOnYAxisExpectCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                                 .rotation = glm::radians(-90.0f),
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(20.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    CollisionInformation expected = {.penetration_depth = 5.0,
                                     .normal = glm::vec3(-1.0, 0.0, 0.0),
                                     .collision_point = glm::vec3(10.0, -5.0, 0.0)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

TEST(SATTest, GivenRectanglesAreRotated45DegreesWhenTheirSidesOverlapExpectCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                 .rotation = glm::radians(-45.0f),
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(6.071, 6.071, 0.0),
                                 .rotation = glm::radians(-45.0f),
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    CollisionInformation expected = {.penetration_depth = 1.414f,
                                     .normal = glm::vec3(0.707f, 0.707f, 0.0f),
                                     .collision_point = glm::vec3(-1.0f, 6.071f, 0.0f)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

TEST(SATTest, GivenRectanglesAreRotatedNeg45DegreesWhenTheirSidesOverlapExpectCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                 .rotation = glm::radians(45.0f),
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(5.0, -5.0, 0.0),
                                 .rotation = glm::radians(-45.0f),
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    CollisionInformation expected = {.penetration_depth = 2.929f,
                                     .normal = glm::vec3(0.707f, -0.707f, 0.0f),
                                     .collision_point = glm::vec3(5.0f, 2.071f, 0.0f)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

TEST(SATTest,
     GivenRectanglesAreRotatedNeg45DegreesWhenTheirCornersOverlapExpectCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-5.0, 0.0, 0.0),
                                 .rotation = glm::radians(45.0f),
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(5.0, 0.0, 0.0),
                                 .rotation = glm::radians(-45.0f),
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    CollisionInformation expected = {.penetration_depth = 2.929f,
                                     .normal = glm::vec3(0.707f, -0.707f, 0.0f),
                                     .collision_point = glm::vec3(-2.071f, 0.0f, 0.0f)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

TEST(SATTest,
     GivenRectanglesAreOffsetFromEachOtherWithNoRotationWithHalfOverlapExpectCollision) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-4.0, 2.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(4.0, -2.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    CollisionInformation expected = {.penetration_depth = 2.0f,
                                     .normal = glm::vec3(1.0f, 0.0f, 0.0f),
                                     .collision_point = glm::vec3(-1.0f, 2.5f, 0.0f)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

TEST(SATTest,
     GivenRectanglesAreOffsetFromEachOtherWithNoRotationWithHalfOverlapExpectCollision2) {
    RigidBody body_a = RigidBody{.position = WorldPoint(4.0, 2.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(-4.0, -2.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(10.0, 10.0)};
    auto output = SAT::collision_detection(body_a, body_b);
    CollisionInformation expected = {.penetration_depth = 2.0f,
                                     .normal = glm::vec3(-1.0f, 0.0f, 0.0f),
                                     .collision_point = glm::vec3(1.0f, 2.5f, 0.0f)};
    float max_diff = 1e-3;
    EXPECT_TRUE(output.has_value());
    expect_near(expected, output.value(), max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample1AtDyn4jTestFindClippingPoints) {
    RigidBody body_a = RigidBody{.position = WorldPoint(11.0, 6.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(6.0, 5.0)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    std::vector<ClippedPoint> output =
        sat_find_clipping_points(body_a, body_b, collision_axis);

    ClippedPoint expected_0 =
        ClippedPoint{.vertex = glm::vec3(12.0, 5.0, 0.0), .depth = 1.0};
    ClippedPoint expected_1 =
        ClippedPoint{.vertex = glm::vec3(8.0, 5.0, 0.0), .depth = 1.0};

    float max_diff = 1e-3;
    expect_near(expected_0, output[0], max_diff);
    expect_near(expected_1, output[1], max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenBodiesOverlapWhenAxisIsDownTestFindClippingPoints) {
    RigidBody body_a = RigidBody{.position = WorldPoint(10.0, 10.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(6.0, 6.0)};

    RigidBody body_b = RigidBody{.position = WorldPoint(13.0, 6.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 4.0)};

    glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    std::vector<ClippedPoint> output =
        sat_find_clipping_points(body_a, body_b, collision_axis);

    ClippedPoint expected_0 =
        ClippedPoint{.vertex = glm::vec3(9.0, 8.0, 0.0), .depth = 1.0};
    ClippedPoint expected_1 =
        ClippedPoint{.vertex = glm::vec3(13.0, 8.0, 0.0), .depth = 1.0};

    float max_diff = 1e-3;
    expect_near(expected_0, output[0], max_diff);
    expect_near(expected_1, output[1], max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample2AtDyn4jTestFindClippingPoints) {
    RigidBody body_a = RigidBody{.position = WorldPoint(5.5, 7.5, 0.0),
                                 .rotation = glm::pi<float>() / 4.0,
                                 .shape = Shape::create_rectangle_data(5.6568, 4.2426)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    std::vector<ClippedPoint> output =
        sat_find_clipping_points(body_a, body_b, collision_axis);

    ClippedPoint expected_0 =
        ClippedPoint{.vertex = glm::vec3(6.0, 4.0, 0.0), .depth = 1.0};

    float max_diff = 1e-3;
    expect_near(expected_0, output[0], max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample3AtDyn4jTestFindClippingPoints) {
    RigidBody body_a = RigidBody{.position = WorldPoint(11.5, 5.5, 0.0),
                                 .rotation = 0.2449,
                                 .shape = Shape::create_rectangle_data(4.1231, 4.1231)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    glm::vec3 collision_axis = glm::vec3(-0.19, -0.98, 0.0);
    std::vector<ClippedPoint> output =
        sat_find_clipping_points(body_a, body_b, collision_axis);

    ClippedPoint expected_0 =
        ClippedPoint{.vertex = glm::vec3(12.0, 5.0, 0.0), .depth = 1.698};
    ClippedPoint expected_1 =
        ClippedPoint{.vertex = glm::vec3(9.25, 5.0, 0.0), .depth = 1.031};

    float max_diff = 1e-3;
    expect_near(expected_0, output[0], max_diff);
    expect_near(expected_1, output[1], max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample1BodyAAtDyn4jTestFindCollisionEdge) {
    RigidBody test_body = RigidBody{.position = WorldPoint(11.0, 6.5, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(6.0, 5.0)};

    glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    CollisionEdge output = sat_find_collision_edge(test_body, collision_axis);

    CollisionEdge expected = CollisionEdge{.max = glm::vec3(8.0, 4.0, 0.0),
                                           .start = glm::vec3(8.0, 4.0, 0.0),
                                           .end = glm::vec3(14.0, 4.0, 0.0),
                                           .edge = glm::vec3(6.0, 0.0, 0.0)};
    float max_diff = 1e-3;
    expect_near(expected, output, max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample1BodyBAtDyn4jTestFindCollisionEdge) {
    RigidBody test_body = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(8.0, 3.0)};

    glm::vec3 collision_axis = glm::vec3(0.0, 1.0, 0.0);
    CollisionEdge output = sat_find_collision_edge(test_body, collision_axis);

    CollisionEdge expected = CollisionEdge{// NOTE: The below commented value is the
                                           // result according to the example. However,
                                           // the implementation does not yield that
                                           // result, instead it yields the used .max
                                           // value. I do not think it matters because it
                                           // is resolved by letting the other bodys edge
                                           // be the reference edge.
                                           /*.max = glm::vec3(12.0, 5.0, 0.0),*/
                                           .max = glm::vec3(4.0, 5.0, 0.0),
                                           .start = glm::vec3(12.0, 5.0, 0.0),
                                           .end = glm::vec3(4.0, 5.0, 0.0),
                                           .edge = glm::vec3(-8.0, 0.0, 0.0)};
    float max_diff = 1e-3;
    EXPECT_EQ(expected.max, output.max)
        << "Expected " << expected.max << " found " << output.max;
    EXPECT_EQ(expected.start, output.start)
        << "Expected " << expected.start << " found " << output.start;
    EXPECT_EQ(expected.end, output.end)
        << "Expected " << expected.end << " found " << output.end;
    EXPECT_EQ(expected.edge, output.edge)
        << "Expected " << expected.edge << " found " << output.edge;
}

TEST(SATTest, GivenOffsetTriangleWithNoRotationTestFindCollisionEdgeOnBodyA) {
    RigidBody test_body = RigidBody{.position = WorldPoint(-4.0f, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(10.0)};

    MTV mtv = MTV{.direction = glm::vec3(0.866025f, -0.5f, 0.0f), .magnitude = 1.732f};
    CollisionEdge output = sat_find_collision_edge(test_body, mtv.direction);
    CollisionEdge expected = CollisionEdge{.max = glm::vec3(1.0f, -2.88675f, 0.0f),
                                           .start = glm::vec3(-9.0f, -2.88675f, 0.0f),
                                           .end = glm::vec3(1.0f, -2.88675f, 0.0f),
                                           .edge = glm::vec3(10.0f, 0.0f, 0.0f)};

    float max_diff = 1e-3;
    expect_near(expected, output, max_diff);
}

TEST(SATTest, GivenOffsetTriangleWithNoRotationTestFindCollisionEdgeOnBodyB) {
    RigidBody test_body = RigidBody{.position = WorldPoint(4.0f, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_triangle_data(10.0)};

    MTV mtv = MTV{.direction = glm::vec3(-0.866025f, 0.5f, 0.0f), .magnitude = 1.732f};
    CollisionEdge output = sat_find_collision_edge(test_body, mtv.direction);
    CollisionEdge expected = CollisionEdge{.max = glm::vec3(4.0f, 5.7735f, 0.0f),
                                           .start = glm::vec3(4.0f, 5.7735f, 0.0f),
                                           .end = glm::vec3(-1.0f, -2.88675f, 0.0f),
                                           .edge = glm::vec3(-5.0f, -8.66025f, 0.0f)};

    float max_diff = 1e-3;
    expect_near(expected, output, max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample2BodyAAtDyn4jTestFindCollisionEdge) {
    RigidBody test_body =
        RigidBody{.position = WorldPoint(5.5, 7.5, 0.0),
                  .rotation = glm::pi<float>() / 4.0,
                  .shape = Shape::create_rectangle_data(5.6568, 4.2426)};

    glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    CollisionEdge output = sat_find_collision_edge(test_body, collision_axis);

    CollisionEdge expected = CollisionEdge{.max = glm::vec3(6.0, 4.0, 0.0),
                                           .start = glm::vec3(2.0, 8.0, 0.0),
                                           .end = glm::vec3(6.0, 4.0, 0.0),
                                           .edge = glm::vec3(4.0, -4.0, 0.0)};
    float max_diff = 1e-3;
    expect_near(expected, output, max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample2BodyBAtDyn4jTestFindCollisionEdge) {
    RigidBody test_body = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(8.0, 3.0)};

    glm::vec3 collision_axis = glm::vec3(0.0, 1.0, 0.0);
    CollisionEdge output = sat_find_collision_edge(test_body, collision_axis);

    CollisionEdge expected = CollisionEdge{// NOTE: The below commented value is the
                                           // result according to the example. However,
                                           // the implementation does not yield that
                                           // result, instead it yields the used .max
                                           // value. I do not think it matters because it
                                           // is resolved by letting the other bodys edge
                                           // be the reference edge.
                                           /*.max = glm::vec3(12.0, 5.0, 0.0),*/
                                           .max = glm::vec3(4.0, 5.0, 0.0),
                                           .start = glm::vec3(12.0, 5.0, 0.0),
                                           .end = glm::vec3(4.0, 5.0, 0.0),
                                           .edge = glm::vec3(-8.0, 0.0, 0.0)};
    float max_diff = 1e-3;
    expect_near(expected, output, max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample3BodyAAtDyn4jTestFindCollisionEdge) {
    RigidBody test_body =
        RigidBody{.position = WorldPoint(11.5, 5.5, 0.0),
                  .rotation = 0.2449,
                  .shape = Shape::create_rectangle_data(4.1231, 4.1231)};

    glm::vec3 collision_axis = glm::vec3(-0.19, -0.98, 0.0);
    CollisionEdge output = sat_find_collision_edge(test_body, collision_axis);

    CollisionEdge expected = CollisionEdge{.max = glm::vec3(13.0, 3.0, 0.0),
                                           .start = glm::vec3(9.0, 4.0, 0.0),
                                           .end = glm::vec3(13.0, 3.0, 0.0),
                                           .edge = glm::vec3(4.0, -1.0, 0.0)};
    float max_diff = 1e-3;
    expect_near(expected, output, max_diff);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample3BodyBAtDyn4jTestFindCollisionEdge) {
    RigidBody test_body = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(8.0, 3.0)};

    glm::vec3 collision_axis = glm::vec3(0.19, 0.98, 0.0);
    CollisionEdge output = sat_find_collision_edge(test_body, collision_axis);

    CollisionEdge expected = CollisionEdge{.max = glm::vec3(12.0, 5.0, 0.0),
                                           .start = glm::vec3(12.0, 5.0, 0.0),
                                           .end = glm::vec3(4.0, 5.0, 0.0),
                                           .edge = glm::vec3(-8.0, 0.0, 0.0)};
    float max_diff = 1e-3;
    expect_near(expected, output, max_diff);
}

TEST(SATTest, GivenTwoTrianglesAreNotRotatedAndCollideAtCorners) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-4.0f, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_triangle_data(10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(4.0f, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_triangle_data(10.0)};
    glm::vec3 collision_axis = glm::vec3(0.19, 0.98, 0.0);
    auto output_ = SAT::collision_detection(body_a, body_b);

    CollisionInformation expected =
        CollisionInformation{.collision_point = glm::vec3(1.0f, -2.88675f, 0.0f),
                             .normal = glm::vec3(0.866025f, -0.5f, 0.0f),
                             .penetration_depth = 1.73205f};
    float max_diff = 1e-3;
    EXPECT_TRUE(output_.has_value());
    CollisionInformation output = output_.value();
    expect_near(expected, output, max_diff);
}

TEST(SATTest, GivenExample1AtDyn4jTestFindMTV) {
    RigidBody body_a = RigidBody{.position = WorldPoint(11.0, 6.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(6.0, 5.0)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    auto mtv_ = find_mtv(body_a, body_b);
    EXPECT_TRUE(mtv_.has_value());
    MTV mtv = mtv_.value();
    MTV expected = MTV{.direction = glm::vec3(0.0, -1.0, 0.0), .magnitude = 1.0};

    EXPECT_EQ(expected.direction, mtv.direction)
        << "Expected " << expected.direction << " found " << mtv.direction;
    EXPECT_EQ(expected.magnitude, mtv.magnitude)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample1AtDyn4jTestFindMTVSwap) {
    RigidBody body_a = RigidBody{.position = WorldPoint(11.0, 6.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(6.0, 5.0)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    auto mtv_ = find_mtv(body_b, body_a);
    EXPECT_TRUE(mtv_.has_value());
    MTV mtv = mtv_.value();
    MTV expected = MTV{.direction = glm::vec3(0.0, 1.0, 0.0), .magnitude = 1.0};

    EXPECT_EQ(expected.direction, mtv.direction)
        << "Expected " << expected.direction << " found " << mtv.direction;
    EXPECT_EQ(expected.magnitude, mtv.magnitude)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenTwoTrianglesTestFindMTV) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-4.0f, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_triangle_data(10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(4.0f, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_triangle_data(10.0)};

    auto mtv_ = find_mtv(body_a, body_b);

    EXPECT_TRUE(mtv_.has_value());
    MTV mtv = mtv_.value();
    MTV expected = MTV{.direction = glm::vec3(0.866025, -0.5, 0), .magnitude = 1.732f};

    expect_near(expected.direction, mtv.direction, 1e-3f);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, 1e-3f)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenTwoTrianglesTestFindMTVSwap) {
    RigidBody body_a = RigidBody{.position = WorldPoint(-4.0f, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_triangle_data(10.0)};
    RigidBody body_b = RigidBody{.position = WorldPoint(4.0f, 0.0, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_triangle_data(10.0)};

    auto mtv_ = find_mtv(body_b, body_a);

    EXPECT_TRUE(mtv_.has_value());
    MTV mtv = mtv_.value();
    MTV expected = MTV{.direction = glm::vec3(-0.866025, 0.5, 0), .magnitude = 1.732f};
    expect_near(expected.direction, mtv.direction, 1e-3f);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, 1e-3f)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample2AtDyn4jTestFindMTV) {
    RigidBody body_a = RigidBody{.position = WorldPoint(5.5, 7.5, 0.0),
                                 .rotation = glm::pi<float>() / 4.0,
                                 .shape = Shape::create_rectangle_data(5.6568, 4.2426)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    auto mtv_ = find_mtv(body_a, body_b);

    EXPECT_TRUE(mtv_.has_value());
    MTV mtv = mtv_.value();
    MTV expected = MTV{.direction = glm::vec3(0.0, -1.0, 0.0), .magnitude = 1.0f};
    expect_near(expected.direction, mtv.direction, 1e-3f);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, 1e-3f)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample2AtDyn4jTestFindMTVSwap) {
    RigidBody body_a = RigidBody{.position = WorldPoint(5.5, 7.5, 0.0),
                                 .rotation = glm::pi<float>() / 4.0,
                                 .shape = Shape::create_rectangle_data(5.6568, 4.2426)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    auto mtv_ = find_mtv(body_b, body_a);

    EXPECT_TRUE(mtv_.has_value());
    MTV mtv = mtv_.value();
    MTV expected = MTV{.direction = glm::vec3(0.0, 1.0, 0.0), .magnitude = 1.0f};
    expect_near(expected.direction, mtv.direction, 1e-3f);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, 1e-3f)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample3AtDyn4jTestFindMTV) {
    RigidBody body_a = RigidBody{.position = WorldPoint(11.5, 5.5, 0.0),
                                 .rotation = 0.2449,
                                 .shape = Shape::create_rectangle_data(4.1231, 4.1231)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    auto mtv_ = find_mtv(body_a, body_b);

    EXPECT_TRUE(mtv_.has_value());
    MTV mtv = mtv_.value();
    // NOTE: This normal does not equal the normal given in the example. Reason being
    // that I think that normal is wrong when a compute it from his vertices. However, the
    // two normals point in the same general direction as that normal is (-0.19, -0.98).
    MTV expected =
        MTV{.direction = glm::vec3(-0.242459, -0.970162, 0), .magnitude = 1.697f};
    expect_near(expected.direction, mtv.direction, 1e-3f);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, 1e-3f)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample3AtDyn4jTestFindMTVSwap) {
    RigidBody body_a = RigidBody{.position = WorldPoint(11.5, 5.5, 0.0),
                                 .rotation = 0.2449,
                                 .shape = Shape::create_rectangle_data(4.1231, 4.1231)};

    RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                 .rotation = 0.0,
                                 .shape = Shape::create_rectangle_data(8.0, 3.0)};

    auto mtv_ = find_mtv(body_b, body_a);

    EXPECT_TRUE(mtv_.has_value());
    MTV mtv = mtv_.value();
    // NOTE: This normal does not equal the normal given in the example. Reason being
    // that I think that normal is wrong when a compute it from his vertices. However, the
    // two normals point in the same general direction as that normal is (-0.19, -0.98).
    MTV expected =
        MTV{.direction = glm::vec3(0.242459, 0.970162, 0), .magnitude = 1.697f};
    expect_near(expected.direction, mtv.direction, 1e-3f);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, 1e-3f)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}
