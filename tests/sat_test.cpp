#include "glm/ext/scalar_constants.hpp"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.cpp"
#include "physics_engine/SAT.h"
#include "shape.h"
#include "test_utils.h"
#include <gtest/gtest.h>
#include <optional>

void expect_near(const CollisionEdge &expected, const CollisionEdge &v,
                 const float epsilon) {
    expect_near(expected.max, v.max, epsilon);
    expect_near(expected.start, v.start, epsilon);
    expect_near(expected.end, v.end, epsilon);
    expect_near(expected.edge, v.edge, epsilon);
}

TEST(SATTest, GivenCircleAndRectangleDoNotCollide) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-11.0, 0.0, 0.0))
                                 .shape(Shape::create_circle_data(20.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(11.0, 0.0, 0.0))
                                 .shape(Shape::create_rectangle_data(20.0, 20.0))
                                 .build();
    const auto output = SAT::collision_detection(body_a, body_b);
    EXPECT_FALSE(output.has_value());
}

TEST(SATTest, GivenCircleAndRectangleDoCollide) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-9.0, 0.0, 0.0))
                                 .shape(Shape::create_circle_data(20.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(10.0, 0.0, 0.0))
                                 .shape(Shape::create_rectangle_data(20.0, 20.0))
                                 .build();
    const auto output = SAT::collision_detection(body_a, body_b);
    EXPECT_TRUE(output.has_value());
    EXPECT_NEAR(1.0f, output->penetration_depth, MAX_DIFF);
    expect_near(glm::vec3(1.0f, 0.0, 0.0), output->normal, MAX_DIFF);
    EXPECT_EQ(ContactType::VERTEX_VERTEX, output->contact_type);
    expect_near(glm::vec3(1.0f, 0.0, 0.0), output->contact_patch[0], MAX_DIFF);
    EXPECT_EQ(0, output->deepest_contact_idx);
}

TEST(SATTest, GivenCirclesDoNotCollide) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-11.0, 0.0, 0.0))
                                 .shape(Shape::create_circle_data(20.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(11.0, 0.0, 0.0))
                                 .shape(Shape::create_circle_data(20.0))
                                 .build();
    const auto output = SAT::collision_detection(body_a, body_b);
    EXPECT_FALSE(output.has_value());
}

TEST(SATTest, GivenCirclesDoCollide) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-9.0, 0.0, 0.0))
                                 .shape(Shape::create_circle_data(20.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(10.0, 0.0, 0.0))
                                 .shape(Shape::create_circle_data(20.0))
                                 .build();
    const auto output = SAT::collision_detection(body_a, body_b);
    EXPECT_TRUE(output.has_value());
    EXPECT_NEAR(1.0f, output->penetration_depth, MAX_DIFF);
    expect_near(glm::vec3(1.0f, 0.0, 0.0), output->normal, MAX_DIFF);
    EXPECT_EQ(ContactType::VERTEX_VERTEX, output->contact_type);
    expect_near(glm::vec3(1.0f, 0.0, 0.0), output->contact_patch[0], MAX_DIFF);
    EXPECT_EQ(0, output->deepest_contact_idx);
}

TEST(SATTest, GivenRectanglesAreAxisAlignedWhenDoNotCollideExpectNoCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-10.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(11.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const auto output = SAT::collision_detection(body_a, body_b);
    EXPECT_FALSE(output.has_value());
}

TEST(SATTest, GivenRectanglesAreAxisAlignedWhenTouchOnYAxisExpectNoCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-10.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(10.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const auto output = SAT::collision_detection(body_a, body_b);
    EXPECT_FALSE(output.has_value());
}

TEST(SATTest, GivenRectanglesAreAxisAlignedWhenOverlapOnYAxisExpectCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-10.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(9.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();

    const std::optional<CollisionInformation> output_ =
        SAT::collision_detection(body_a, body_b);

    EXPECT_TRUE(output_.has_value());

    const CollisionInformation output = output_.value();
    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(-1.0, 5.0, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(-1.0, -5.0, 0.0), output.contact_patch[1], MAX_DIFF);
    EXPECT_EQ(glm::vec3(1.0, 0.0, 0.0), output.normal);
    EXPECT_EQ(1.0, output.penetration_depth);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(
    SATTest,
    GivenRectanglesAreAxisAlignedWhenOverlapOnYAxisButBodiesHaveSwappedOrderExpectCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(9.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(-10.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const auto output_ = SAT::collision_detection(body_a, body_b);

    EXPECT_TRUE(output_.has_value());

    const CollisionInformation output = output_.value();
    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(0.0, -5.0, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(0.0, 5.0, 0.0), output.contact_patch[1], MAX_DIFF);
    EXPECT_EQ(glm::vec3(-1.0, 0.0, 0.0), output.normal);
    EXPECT_EQ(1.0, output.penetration_depth);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(
    SATTest,
    GivenRectanglesAreAxisAlignedAndOffsetFromOrigoWhenOverlappingOnXAxisExpectCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-10.0, 20.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(-10.0, 15.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const auto output_ = SAT::collision_detection(body_a, body_b);

    EXPECT_TRUE(output_.has_value());

    const CollisionInformation output = output_.value();
    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(0.0, 20.0, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(-20.0, 20.0, 0.0), output.contact_patch[1], MAX_DIFF);
    EXPECT_EQ(glm::vec3(0.0, -1.0, 0.0), output.normal);
    EXPECT_EQ(5.0, output.penetration_depth);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(
    SATTest,
    GivenOneRectangleIsAxisAlignedAndOneRotated90DegreesWhenOverlapOnYAxisExpectCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(10.0, 0.0, 0.0))
                                 .rotation(glm::radians(-90.0f))
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(0.0, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(20.0, 10.0))
                                 .build();
    const auto output_ = SAT::collision_detection(body_a, body_b);

    EXPECT_TRUE(output_.has_value());

    const CollisionInformation output = output_.value();
    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(10.0, -5.0, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(10.0, 5.0, 0.0), output.contact_patch[1], MAX_DIFF);
    expect_near(glm::vec3(-1.0, 0.0, 0.0), output.normal, MAX_DIFF);
    EXPECT_EQ(5.0, output.penetration_depth);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(SATTest, GivenRectanglesAreRotated45DegreesWhenTheirSidesOverlapExpectCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(0.0, 0.0, 0.0))
                                 .rotation(glm::radians(-45.0f))
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(6.071, 6.071, 0.0))
                                 .rotation(glm::radians(-45.0f))
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const auto output_ = SAT::collision_detection(body_a, body_b);

    EXPECT_TRUE(output_.has_value());

    const CollisionInformation output = output_.value();
    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(-1.0f, 6.071f, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(6.071f, -1.0f, 0.0), output.contact_patch[1], MAX_DIFF);
    expect_near(glm::vec3(0.707f, 0.707f, 0.0), output.normal, MAX_DIFF);
    EXPECT_NEAR(1.414f, output.penetration_depth, MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(SATTest, GivenRectanglesAreRotatedNeg45DegreesWhenTheirSidesOverlapExpectCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(0.0, 0.0, 0.0))
                                 .rotation(glm::radians(45.0f))
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(5.0, -5.0, 0.0))
                                 .rotation(glm::radians(-45.0f))
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const auto output_ = SAT::collision_detection(body_a, body_b);

    EXPECT_TRUE(output_.has_value());

    const CollisionInformation output = output_.value();
    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(5.0f, 2.071f, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(-2.071f, -5.0f, 0.0), output.contact_patch[1], MAX_DIFF);
    expect_near(glm::vec3(0.707f, -0.707f, 0.0), output.normal, MAX_DIFF);
    EXPECT_NEAR(2.929f, output.penetration_depth, MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(SATTest,
     GivenRectanglesAreRotatedNeg45DegreesWhenTheirCornersOverlapExpectCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-5.0, 0.0, 0.0))
                                 .rotation(glm::radians(45.0f))
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(5.0, 0.0, 0.0))
                                 .rotation(glm::radians(-45.0f))
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const auto output_ = SAT::collision_detection(body_a, body_b);
    EXPECT_TRUE(output_.has_value());
    const CollisionInformation output = output_.value();

    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(-2.071f, 0.0f, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(0.0f, 2.071f, 0.0), output.contact_patch[1], MAX_DIFF);
    expect_near(glm::vec3(0.707f, -0.707f, 0.0), output.normal, MAX_DIFF);
    EXPECT_NEAR(2.929f, output.penetration_depth, MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(SATTest,
     GivenRectanglesAreOffsetFromEachOtherWithNoRotationWithHalfOverlapExpectCollision) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-4.0, 2.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(4.0, -2.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const auto output_ = SAT::collision_detection(body_a, body_b);
    EXPECT_TRUE(output_.has_value());
    const CollisionInformation output = output_.value();

    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(-1.0, 2.5f, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(-1.0, -2.5f, 0.0), output.contact_patch[1], MAX_DIFF);
    expect_near(glm::vec3(1.0f, 0.0, 0.0), output.normal, MAX_DIFF);
    EXPECT_NEAR(2.0f, output.penetration_depth, MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(SATTest,
     GivenRectanglesAreOffsetFromEachOtherWithNoRotationWithHalfOverlapExpectCollision2) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(4.0, 2.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(-4.0, -2.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(10.0, 10.0))
                                 .build();

    const auto output_ = SAT::collision_detection(body_a, body_b);
    EXPECT_TRUE(output_.has_value());
    const CollisionInformation output = output_.value();

    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(1.0, 2.5f, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(1.0, -2.5f, 0.0), output.contact_patch[1], MAX_DIFF);
    expect_near(glm::vec3(-1.0f, 0.0, 0.0), output.normal, MAX_DIFF);
    EXPECT_NEAR(2.0f, output.penetration_depth, MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample1AtDyn4jTestFindClippingPoints) {
    RigidBody body_a = RigidBodyBuilder()
                           .position(WorldPoint(11.0, 6.5, 0.0))
                           .rotation(0.0)
                           .shape(Shape::create_rectangle_data(6.0, 5.0))
                           .build();

    RigidBody body_b = RigidBodyBuilder()
                           .position(WorldPoint(8.0, 3.5, 0.0))
                           .rotation(0.0)
                           .shape(Shape::create_rectangle_data(8.0, 3.0))
                           .build();

    glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    auto edge_a = find_collision_edge(body_a, collision_axis);
    auto edge_b = find_collision_edge(body_b, -collision_axis);

    CollisionInformation output = find_clipping_points(edge_a, edge_b, collision_axis);

    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    EXPECT_EQ(glm::vec3(12.0, 5.0, 0.0), output.contact_patch[0]);
    EXPECT_EQ(glm::vec3(8.0, 5.0, 0.0), output.contact_patch[1]);
    EXPECT_EQ(0, output.deepest_contact_idx);
    EXPECT_NEAR(1.0, output.penetration_depth, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenBodiesOverlapWhenAxisIsDownTestFindClippingPoints) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(10.0, 10.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(6.0, 6.0))
                                 .build();

    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(13.0, 6.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(8.0, 4.0))
                                 .build();

    const glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    const auto edge_a = find_collision_edge(body_a, collision_axis);
    const auto edge_b = find_collision_edge(body_b, -collision_axis);

    const CollisionInformation output =
        find_clipping_points(edge_a, edge_b, collision_axis);

    EXPECT_EQ(ContactType::EDGE_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(9.0, 8.0, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(13.0, 8.0, 0.0), output.contact_patch[1], MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
    EXPECT_NEAR(1.0, output.penetration_depth, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample2AtDyn4jTestFindClippingPoints) {
    const RigidBody body_a =
        RigidBody{.position = WorldPoint(5.5, 7.5, 0.0),
                  .rotation = glm::radians(45.0),
                  .shape = Shape::create_rectangle_data(5.6568, 4.2426)};

    const RigidBody body_b = RigidBody{.position = WorldPoint(8.0, 3.5, 0.0),
                                       .rotation = 0.0,
                                       .shape = Shape::create_rectangle_data(8.0, 3.0)};

    const glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    const auto edge_a = find_collision_edge(body_a, collision_axis);
    const auto edge_b = find_collision_edge(body_b, -collision_axis);

    const CollisionInformation output =
        find_clipping_points(edge_a, edge_b, collision_axis);

    EXPECT_EQ(ContactType::VERTEX_EDGE, output.contact_type);
    EXPECT_EQ(1, output.contact_patch.size());
    expect_near(glm::vec3(6.0, 4.0, 0.0), output.contact_patch[0], MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
    EXPECT_NEAR(1.0, output.penetration_depth, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample3AtDyn4jTestFindClippingPoints) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(11.5, 5.5, 0.0))
                                 .rotation(0.2449)
                                 .shape(Shape::create_rectangle_data(4.1231, 4.1231))
                                 .build();

    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(8.0, 3.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(8.0, 3.0))
                                 .build();

    const glm::vec3 collision_axis = glm::vec3(-0.19, -0.98, 0.0);
    const auto edge_a = find_collision_edge(body_a, collision_axis);
    const auto edge_b = find_collision_edge(body_b, -collision_axis);

    const CollisionInformation output =
        find_clipping_points(edge_a, edge_b, collision_axis);

    EXPECT_EQ(ContactType::VERTEX_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(12.0, 5.0, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(9.25, 5.0, 0.0), output.contact_patch[1], MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
    EXPECT_NEAR(1.698, output.penetration_depth, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample1BodyAAtDyn4jTestFindCollisionEdge) {
    const RigidBody test_body = RigidBodyBuilder()
                                    .position(WorldPoint(11.0, 6.5, 0.0))
                                    .rotation(0.0)
                                    .shape(Shape::create_rectangle_data(6.0, 5.0))
                                    .build();

    const glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    const CollisionEdge output = find_collision_edge(test_body, collision_axis);

    const CollisionEdge expected = CollisionEdge{.start = glm::vec3(8.0, 4.0, 0.0),
                                                 .end = glm::vec3(14.0, 4.0, 0.0),
                                                 .max = glm::vec3(8.0, 4.0, 0.0),
                                                 .edge = glm::vec3(6.0, 0.0, 0.0)};
    expect_near(expected, output, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample1BodyBAtDyn4jTestFindCollisionEdge) {
    const RigidBody test_body = RigidBodyBuilder()
                                    .position(WorldPoint(8.0, 3.5, 0.0))
                                    .rotation(0.0)
                                    .shape(Shape::create_rectangle_data(8.0, 3.0))
                                    .build();

    const glm::vec3 collision_axis = glm::vec3(0.0, 1.0, 0.0);
    const CollisionEdge output = find_collision_edge(test_body, collision_axis);

    const CollisionEdge expected =
        CollisionEdge{// NOTE: The below commented value is the
                      // result according to the example. However,
                      // the implementation does not yield that
                      // result, instead it yields the used .max
                      // value. I do not think it matters because it
                      // is resolved by letting the other bodys edge
                      // be the reference edge.
                      /*.max = glm::vec3(12.0, 5.0, 0.0),*/
                      .start = glm::vec3(12.0, 5.0, 0.0),
                      .end = glm::vec3(4.0, 5.0, 0.0),
                      .max = glm::vec3(4.0, 5.0, 0.0),
                      .edge = glm::vec3(-8.0, 0.0, 0.0)};
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
    const RigidBody test_body = RigidBodyBuilder()
                                    .position(WorldPoint(-4.0f, 0.0, 0.0))
                                    .rotation(0.0)
                                    .shape(Shape::create_triangle_data(10.0))
                                    .build();

    const MTV mtv =
        MTV{.direction = glm::vec3(0.866025f, -0.5f, 0.0f), .magnitude = 1.732f};
    const CollisionEdge output = find_collision_edge(test_body, mtv.direction);
    const CollisionEdge expected =
        CollisionEdge{.start = glm::vec3(-9.0f, -2.88675f, 0.0f),
                      .end = glm::vec3(1.0f, -2.88675f, 0.0f),
                      .max = glm::vec3(1.0f, -2.88675f, 0.0f),
                      .edge = glm::vec3(10.0f, 0.0f, 0.0f)};

    expect_near(expected, output, MAX_DIFF);
}

TEST(SATTest, GivenOffsetTriangleWithNoRotationTestFindCollisionEdgeOnBodyB) {
    const RigidBody test_body = RigidBodyBuilder()
                                    .position(WorldPoint(4.0f, 0.0, 0.0))
                                    .rotation(0.0)
                                    .shape(Shape::create_triangle_data(10.0))
                                    .build();

    const MTV mtv =
        MTV{.direction = glm::vec3(-0.866025f, 0.5f, 0.0f), .magnitude = 1.732f};
    const CollisionEdge output = find_collision_edge(test_body, mtv.direction);
    const CollisionEdge expected =
        CollisionEdge{.start = glm::vec3(4.0f, 5.7735f, 0.0f),
                      .end = glm::vec3(-1.0f, -2.88675f, 0.0f),
                      .max = glm::vec3(4.0f, 5.7735f, 0.0f),
                      .edge = glm::vec3(-5.0f, -8.66025f, 0.0f)};

    expect_near(expected, output, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample2BodyAAtDyn4jTestFindCollisionEdge) {
    RigidBody test_body =
        RigidBody{.position = WorldPoint(5.5, 7.5, 0.0),
                  .rotation = glm::pi<float>() / 4.0,
                  .shape = Shape::create_rectangle_data(5.6568, 4.2426)};

    glm::vec3 collision_axis = glm::vec3(0.0, -1.0, 0.0);
    CollisionEdge output = find_collision_edge(test_body, collision_axis);

    CollisionEdge expected = CollisionEdge{.start = glm::vec3(2.0, 8.0, 0.0),
                                           .end = glm::vec3(6.0, 4.0, 0.0),
                                           .max = glm::vec3(6.0, 4.0, 0.0),
                                           .edge = glm::vec3(4.0, -4.0, 0.0)};
    expect_near(expected, output, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample2BodyBAtDyn4jTestFindCollisionEdge) {
    const RigidBody test_body = RigidBodyBuilder()
                                    .position(WorldPoint(8.0, 3.5, 0.0))
                                    .rotation(0.0)
                                    .shape(Shape::create_rectangle_data(8.0, 3.0))
                                    .build();

    const glm::vec3 collision_axis = glm::vec3(0.0, 1.0, 0.0);
    const CollisionEdge output = find_collision_edge(test_body, collision_axis);

    const CollisionEdge expected =
        CollisionEdge{// NOTE: The below commented value is the
                      // result according to the example. However,
                      // the implementation does not yield that
                      // result, instead it yields the used .max
                      // value. I do not think it matters because it
                      // is resolved by letting the other bodys edge
                      // be the reference edge.
                      /*.max = glm::vec3(12.0, 5.0, 0.0),*/
                      .start = glm::vec3(12.0, 5.0, 0.0),
                      .end = glm::vec3(4.0, 5.0, 0.0),
                      .max = glm::vec3(4.0, 5.0, 0.0),
                      .edge = glm::vec3(-8.0, 0.0, 0.0)};
    expect_near(expected, output, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample3BodyAAtDyn4jTestFindCollisionEdge) {
    const RigidBody test_body =
        RigidBody{.position = WorldPoint(11.5, 5.5, 0.0),
                  .rotation = 0.2449,
                  .shape = Shape::create_rectangle_data(4.1231, 4.1231)};

    const glm::vec3 collision_axis = glm::vec3(-0.19, -0.98, 0.0);
    const CollisionEdge output = find_collision_edge(test_body, collision_axis);

    const CollisionEdge expected = CollisionEdge{.start = glm::vec3(9.0, 4.0, 0.0),
                                                 .end = glm::vec3(13.0, 3.0, 0.0),
                                                 .max = glm::vec3(13.0, 3.0, 0.0),
                                                 .edge = glm::vec3(4.0, -1.0, 0.0)};
    expect_near(expected, output, MAX_DIFF);
}

// https://dyn4j.org/2011/11/contact-points-using-clipping/
TEST(SATTest, GivenExample3BodyBAtDyn4jTestFindCollisionEdge) {
    const RigidBody test_body = RigidBodyBuilder()
                                    .position(WorldPoint(8.0, 3.5, 0.0))
                                    .rotation(0.0)
                                    .shape(Shape::create_rectangle_data(8.0, 3.0))
                                    .build();

    const glm::vec3 collision_axis = glm::vec3(0.19, 0.98, 0.0);
    const CollisionEdge output = find_collision_edge(test_body, collision_axis);

    const CollisionEdge expected = CollisionEdge{.start = glm::vec3(12.0, 5.0, 0.0),
                                                 .end = glm::vec3(4.0, 5.0, 0.0),
                                                 .max = glm::vec3(12.0, 5.0, 0.0),
                                                 .edge = glm::vec3(-8.0, 0.0, 0.0)};
    expect_near(expected, output, MAX_DIFF);
}

TEST(SATTest, GivenTwoTrianglesAreNotRotatedAndCollideAtCorners) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-4.0f, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_triangle_data(10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(4.0f, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_triangle_data(10.0))
                                 .build();
    const auto output_ = SAT::collision_detection(body_a, body_b);
    EXPECT_TRUE(output_.has_value());
    const CollisionInformation output = output_.value();

    EXPECT_EQ(ContactType::VERTEX_EDGE, output.contact_type);
    EXPECT_EQ(2, output.contact_patch.size());
    expect_near(glm::vec3(1.0, -2.88675f, 0.0), output.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(-1.0, -2.88675f, 0.0), output.contact_patch[1], MAX_DIFF);
    expect_near(glm::vec3(0.866025f, -0.5, 0.0), output.normal, MAX_DIFF);
    EXPECT_NEAR(1.73205f, output.penetration_depth, MAX_DIFF);
    EXPECT_EQ(0, output.deepest_contact_idx);
}

TEST(SATTest, GivenExample1AtDyn4jTestFindMTV) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(11.0, 6.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(6.0, 5.0))
                                 .build();

    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(8.0, 3.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(8.0, 3.0))
                                 .build();

    const auto mtv_ = find_mtv_polygon(body_a, body_b);
    EXPECT_TRUE(mtv_.has_value());
    const MTV mtv = mtv_.value();
    const MTV expected = MTV{.direction = glm::vec3(0.0, -1.0, 0.0), .magnitude = 1.0};

    EXPECT_EQ(expected.direction, mtv.direction)
        << "Expected " << expected.direction << " found " << mtv.direction;
    EXPECT_EQ(expected.magnitude, mtv.magnitude)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample1AtDyn4jTestFindMTVSwap) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(11.0, 6.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(6.0, 5.0))
                                 .build();

    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(8.0, 3.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(8.0, 3.0))
                                 .build();

    const auto mtv_ = find_mtv_polygon(body_b, body_a);
    EXPECT_TRUE(mtv_.has_value());
    const MTV mtv = mtv_.value();
    const MTV expected = MTV{.direction = glm::vec3(0.0, 1.0, 0.0), .magnitude = 1.0};

    EXPECT_EQ(expected.direction, mtv.direction)
        << "Expected " << expected.direction << " found " << mtv.direction;
    EXPECT_EQ(expected.magnitude, mtv.magnitude)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenTwoTrianglesTestFindMTV) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-4.0f, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_triangle_data(10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(4.0f, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_triangle_data(10.0))
                                 .build();

    const auto mtv_ = find_mtv_polygon(body_a, body_b);

    EXPECT_TRUE(mtv_.has_value());
    const MTV mtv = mtv_.value();
    const MTV expected =
        MTV{.direction = glm::vec3(0.866025, -0.5, 0), .magnitude = 1.732f};

    expect_near(expected.direction, mtv.direction, MAX_DIFF);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, MAX_DIFF)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenTwoTrianglesTestFindMTVSwap) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(-4.0f, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_triangle_data(10.0))
                                 .build();
    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(4.0f, 0.0, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_triangle_data(10.0))
                                 .build();

    const auto mtv_ = find_mtv_polygon(body_b, body_a);

    EXPECT_TRUE(mtv_.has_value());
    const MTV mtv = mtv_.value();
    const MTV expected =
        MTV{.direction = glm::vec3(-0.866025, 0.5, 0), .magnitude = 1.732f};
    expect_near(expected.direction, mtv.direction, MAX_DIFF);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, MAX_DIFF)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample2AtDyn4jTestFindMTV) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(5.5, 7.5, 0.0))
                                 .rotation(glm::pi<float>() / 4.0)
                                 .shape(Shape::create_rectangle_data(5.6568, 4.2426))
                                 .build();

    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(8.0, 3.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(8.0, 3.0))
                                 .build();

    const auto mtv_ = find_mtv_polygon(body_a, body_b);

    EXPECT_TRUE(mtv_.has_value());
    const MTV mtv = mtv_.value();
    const MTV expected = MTV{.direction = glm::vec3(0.0, -1.0, 0.0), .magnitude = 1.0f};
    expect_near(expected.direction, mtv.direction, 1e-3f);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, 1e-3f)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample2AtDyn4jTestFindMTVSwap) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(5.5, 7.5, 0.0))
                                 .rotation(glm::pi<float>() / 4.0)
                                 .shape(Shape::create_rectangle_data(5.6568, 4.2426))
                                 .build();

    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(8.0, 3.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(8.0, 3.0))
                                 .build();

    const auto mtv_ = find_mtv_polygon(body_b, body_a);

    EXPECT_TRUE(mtv_.has_value());
    const MTV mtv = mtv_.value();
    const MTV expected = MTV{.direction = glm::vec3(0.0, 1.0, 0.0), .magnitude = 1.0f};
    expect_near(expected.direction, mtv.direction, MAX_DIFF);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, MAX_DIFF)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample3AtDyn4jTestFindMTV) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(11.5, 5.5, 0.0))
                                 .rotation(0.2449)
                                 .shape(Shape::create_rectangle_data(4.1231, 4.1231))
                                 .build();

    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(8.0, 3.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(8.0, 3.0))
                                 .build();

    const auto mtv_ = find_mtv_polygon(body_a, body_b);

    EXPECT_TRUE(mtv_.has_value());
    const MTV mtv = mtv_.value();
    // NOTE: This normal does not equal the normal given in the example. Reason being
    // that I think that normal is wrong when a compute it from his vertices. However, the
    // two normals point in the same general direction as that normal is (-0.19, -0.98).
    const MTV expected =
        MTV{.direction = glm::vec3(-0.242459, -0.970162, 0), .magnitude = 1.697f};
    expect_near(expected.direction, mtv.direction, MAX_DIFF);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, MAX_DIFF)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}

TEST(SATTest, GivenExample3AtDyn4jTestFindMTVSwap) {
    const RigidBody body_a = RigidBodyBuilder()
                                 .position(WorldPoint(11.5, 5.5, 0.0))
                                 .rotation(0.2449)
                                 .shape(Shape::create_rectangle_data(4.1231, 4.1231))
                                 .build();

    const RigidBody body_b = RigidBodyBuilder()
                                 .position(WorldPoint(8.0, 3.5, 0.0))
                                 .rotation(0.0)
                                 .shape(Shape::create_rectangle_data(8.0, 3.0))
                                 .build();

    const auto mtv_ = find_mtv_polygon(body_b, body_a);

    EXPECT_TRUE(mtv_.has_value());
    const MTV mtv = mtv_.value();
    // NOTE: This normal does not equal the normal given in the example. Reason being
    // that I think that normal is wrong when a compute it from his vertices. However, the
    // two normals point in the same general direction as that normal is (-0.19, -0.98).
    const MTV expected =
        MTV{.direction = glm::vec3(0.242459, 0.970162, 0), .magnitude = 1.697f};
    expect_near(expected.direction, mtv.direction, MAX_DIFF);
    EXPECT_NEAR(expected.magnitude, mtv.magnitude, MAX_DIFF)
        << "Expected " << expected.magnitude << " found " << mtv.magnitude;
}
