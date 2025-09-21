#include "game_engine_sdk/WorldPoint.h"
#include "game_engine_sdk/equations/equations.h"
#include "game_engine_sdk/io.h"
#include "game_engine_sdk/physics_engine/RigidBody.h"
#include "game_engine_sdk/physics_engine/SAT.h"
#include "game_engine_sdk/physics_engine/collision_resolver.h"
#include "test_utils.h"
#include <cfloat>
#include <gtest/gtest.h>

// TODO: There are two issues
// - Impulse seems to be applied in the wrong direction (sign is wrong)
// - I am unsure about the center_to_p vectors, should the not be perpendicular to the
// contact plane?

TEST(CollisionResolverTest,
     GivenTwoRectanglesWhenTheyHaveNoVelocityExpectPositionalChange) {

    RigidBody body_a = RigidBodyBuilder()
                           .position(WorldPoint(-4.0f, 0.0f, 0.0f))
                           .shape(Shape::create_rectangle_data(10.0f, 10.0f))
                           .mass(1.0)
                           .build();
    RigidBody body_b = RigidBodyBuilder()
                           .position(WorldPoint(4.0f, 0.0f, 0.0f))
                           .shape(Shape::create_rectangle_data(10.0f, 10.0f))
                           .mass(1.0)
                           .build();

    const auto collision_info_ = SAT::collision_detection(body_a, body_b);
    EXPECT_TRUE(collision_info_.has_value());
    const CollisionInformation collision_info = collision_info_.value();

    EXPECT_EQ(ContactType::EDGE_EDGE, collision_info.contact_type);
    EXPECT_EQ(2, collision_info.contact_patch.size());
    expect_near(glm::vec3(-1.0f, 5.0f, 0.0f), collision_info.contact_patch[0], MAX_DIFF);
    expect_near(glm::vec3(-1.0f, -5.0f, 0.0), collision_info.contact_patch[1], MAX_DIFF);
    expect_near(glm::vec3(1.0f, 0.0, 0.0), collision_info.normal, MAX_DIFF);
    EXPECT_NEAR(2.0f, collision_info.penetration_depth, MAX_DIFF);
    EXPECT_EQ(0, collision_info.deepest_contact_idx);

    auto solver = CollisionSolver(1.0);
    auto collision_corrections_ =
        solver.resolve_collision(collision_info, body_a, body_b);
    EXPECT_TRUE(collision_corrections_.has_value());
    CollisionCorrections ccs = collision_corrections_.value();

    glm::vec3 initial_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float initial_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    body_a.position += ccs.body_a.position;
    body_a.velocity += ccs.body_a.velocity;
    body_a.angular_velocity += ccs.body_a.angular_velocity;

    body_b.position += ccs.body_b.position;
    body_b.velocity += ccs.body_b.velocity;
    body_b.angular_velocity += ccs.body_b.angular_velocity;

    glm::vec3 final_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float final_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    expect_near(WorldPoint(-5.0f, 0.0f, 0.0f), body_a.position, MAX_DIFF);
    expect_near(WorldPoint(5.0f, 0.0f, 0.0f), body_b.position, MAX_DIFF);

    EXPECT_EQ(initial_linear_momentum, final_linear_momentum);
    EXPECT_EQ(initial_angular_momentum, final_angular_momentum);
}

TEST(CollisionResolverTest,
     GivenRectAndRectHasCollidedExpectEachBodyToMoveHalfPenetrationDepth) {
    RigidBody body_a = RigidBodyBuilder()
                           .position(WorldPoint(-50.0, 0.0, 0.0))
                           .velocity(glm::vec3(0.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(100.0, 50.0))
                           .build();
    RigidBody body_b = RigidBodyBuilder()
                           .position(WorldPoint(0.0, 0.0, 0.0))
                           .velocity(glm::vec3(0.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(80.0, 80.0))
                           .build();

    std::optional<CollisionInformation> collision_info_ =
        SAT::collision_detection(body_a, body_b);

    EXPECT_TRUE(collision_info_.has_value());

    CollisionInformation collision_info = collision_info_.value();

    EXPECT_EQ(ContactType::EDGE_EDGE, collision_info.contact_type);
    EXPECT_EQ(glm::vec3(1.0f, 0.0f, 0.0f), collision_info.normal);
    EXPECT_EQ(40.0f, collision_info.penetration_depth);
    EXPECT_EQ(glm::vec3(-40.0f, -25.0f, 0.0f), collision_info.contact_patch[0])
        << "collision_info.contact_patch[0] = " << collision_info.contact_patch[0];
    EXPECT_EQ(glm::vec3(-40.0f, 25.0f, 0.0f), collision_info.contact_patch[1])
        << "collision_info.contact_patch[1] = " << collision_info.contact_patch[1];

    auto solver = CollisionSolver(1.0);
    auto collision_corrections_ =
        solver.resolve_collision(collision_info, body_a, body_b);
    EXPECT_TRUE(collision_corrections_.has_value());
    CollisionCorrections ccs = collision_corrections_.value();

    glm::vec3 initial_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float initial_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    body_a.position += ccs.body_a.position;
    body_a.velocity += ccs.body_a.velocity;
    body_a.angular_velocity += ccs.body_a.angular_velocity;

    body_b.position += ccs.body_b.position;
    body_b.velocity += ccs.body_b.velocity;
    body_b.angular_velocity += ccs.body_b.angular_velocity;

    glm::vec3 final_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float final_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    EXPECT_EQ(WorldPoint(-70.0f, 0.0f, 0.0f), body_a.position)
        << "body_a.position = " << body_a.position;
    EXPECT_EQ(WorldPoint(20.0f, 0.0f, 0.0f), body_b.position)
        << "body_b.position = " << body_b.position;
    EXPECT_EQ(glm::vec3(0.0, 0.0, 0.0), body_a.velocity)
        << "body_a.velocity = " << body_a.velocity;
    EXPECT_EQ(glm::vec3(0.0, 0.0, 0.0), body_b.velocity)
        << "body_b.velocity = " << body_b.velocity;
    EXPECT_EQ(0.0, body_a.angular_velocity);
    EXPECT_EQ(0.0, body_b.angular_velocity);

    EXPECT_EQ(initial_linear_momentum, final_linear_momentum);
    EXPECT_EQ(initial_angular_momentum, final_angular_momentum);
}

TEST(CollisionResolverTest, GivenRectAndRectHasCollidedExpectVelocityTransfer) {
    RigidBody body_a = RigidBodyBuilder()
                           .position(WorldPoint(-50.0, 0.0, 0.0))
                           .velocity(glm::vec3(0.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(100.0, 100.0))
                           .build();
    RigidBody body_b = RigidBodyBuilder()
                           .position(WorldPoint(0.0, 0.0, 0.0))
                           .velocity(glm::vec3(-100.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(80.0, 80.0))
                           .build();

    CollisionInformation collision_info{
        .contact_type = ContactType::EDGE_EDGE,
        .normal = glm::vec3(1.0f, 0.0f, 0.0f),
        .contact_patch = {glm::vec3(-40.0f, 40.0f, 0.0), glm::vec3(-40.0f, -40.0f, 0.0)},
        .deepest_contact_idx = 0,
        .penetration_depth = 40.0f};

    std::optional<CollisionInformation> collision_info_ =
        SAT::collision_detection(body_a, body_b);

    auto solver = CollisionSolver(1.0);
    auto collision_corrections_ =
        solver.resolve_collision(collision_info, body_a, body_b);
    EXPECT_TRUE(collision_corrections_.has_value());
    CollisionCorrections ccs = collision_corrections_.value();

    glm::vec3 initial_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float initial_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    body_a.position += ccs.body_a.position;
    body_a.velocity += ccs.body_a.velocity;
    body_a.angular_velocity += ccs.body_a.angular_velocity;

    body_b.position += ccs.body_b.position;
    body_b.velocity += ccs.body_b.velocity;
    body_b.angular_velocity += ccs.body_b.angular_velocity;

    glm::vec3 final_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float final_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    EXPECT_EQ(WorldPoint(-70.0f, 0.0f, 0.0f), body_a.position)
        << "body_a.position = " << body_a.position;
    EXPECT_EQ(WorldPoint(20.0f, 0.0f, 0.0f), body_b.position)
        << "body_b.position = " << body_b.position;
    EXPECT_EQ(glm::vec3(-100.0, 0.0, 0.0), body_a.velocity)
        << "body_a.velocity = " << body_a.velocity;
    EXPECT_EQ(glm::vec3(0.0, 0.0, 0.0), body_b.velocity)
        << "body_b.velocity = " << body_b.velocity;
    EXPECT_EQ(0.0, body_a.angular_velocity);
    EXPECT_EQ(0.0, body_b.angular_velocity);

    EXPECT_EQ(initial_linear_momentum, final_linear_momentum);
    EXPECT_EQ(initial_angular_momentum, final_angular_momentum);
}

TEST(CollisionResolverTest, GivenRectAndRectHasCollidedExpectVelocityTransferReversed) {
    RigidBody body_a = RigidBodyBuilder()
                           .position(WorldPoint(-50.0, 0.0, 0.0))
                           .velocity(glm::vec3(100.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(100.0, 100.0))
                           .build();
    RigidBody body_b = RigidBodyBuilder()
                           .position(WorldPoint(0.0, 0.0, 0.0))
                           .velocity(glm::vec3(0.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(80.0, 80.0))
                           .build();

    CollisionInformation collision_info{
        .contact_type = ContactType::EDGE_EDGE,
        .normal = glm::vec3(1.0f, 0.0f, 0.0f),
        .contact_patch = {glm::vec3(-40.0f, 40.0f, 0.0), glm::vec3(-40.0f, -40.0f, 0.0)},
        .deepest_contact_idx = 0,
        .penetration_depth = 40.0f};

    auto solver = CollisionSolver(1.0);
    auto collision_corrections_ =
        solver.resolve_collision(collision_info, body_a, body_b);
    EXPECT_TRUE(collision_corrections_.has_value());
    CollisionCorrections ccs = collision_corrections_.value();

    glm::vec3 initial_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float initial_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    body_a.position += ccs.body_a.position;
    body_a.velocity += ccs.body_a.velocity;
    body_a.angular_velocity += ccs.body_a.angular_velocity;

    body_b.position += ccs.body_b.position;
    body_b.velocity += ccs.body_b.velocity;
    body_b.angular_velocity += ccs.body_b.angular_velocity;

    glm::vec3 final_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float final_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    EXPECT_EQ(WorldPoint(-70.0f, 0.0f, 0.0f), body_a.position)
        << "body_a.position = " << body_a.position;
    EXPECT_EQ(WorldPoint(20.0f, 0.0f, 0.0f), body_b.position)
        << "body_b.position = " << body_b.position;
    EXPECT_EQ(glm::vec3(0.0, 0.0, 0.0), body_a.velocity)
        << "body_a.velocity = " << body_a.velocity;
    EXPECT_EQ(glm::vec3(100.0, 0.0, 0.0), body_b.velocity)
        << "body_b.velocity = " << body_b.velocity;
    EXPECT_EQ(0.0, body_a.angular_velocity);
    EXPECT_EQ(0.0, body_b.angular_velocity);

    EXPECT_EQ(initial_linear_momentum, final_linear_momentum);
    EXPECT_EQ(initial_angular_momentum, final_angular_momentum);
}

TEST(CollisionResolverTest,
     GivenRectAndRectHasCollidedExpectVelocityTransferSwapedPositions) {
    RigidBody body_a = RigidBodyBuilder()
                           .position(WorldPoint(0.0, 0.0, 0.0))
                           .velocity(glm::vec3(-100.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(100.0, 100.0))
                           .build();
    RigidBody body_b = RigidBodyBuilder()
                           .position(WorldPoint(-50.0, 0.0, 0.0))
                           .velocity(glm::vec3(0.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(80.0, 80.0))
                           .build();

    CollisionInformation collision_info{
        .contact_type = ContactType::EDGE_EDGE,
        .normal = glm::vec3(-1.0f, 0.0f, 0.0f),
        .contact_patch = {glm::vec3(-10.0f, 40.0f, 0.0), glm::vec3(-10.0f, -40.0f, 0.0)},
        .deepest_contact_idx = 0,
        .penetration_depth = 40.0f};

    auto solver = CollisionSolver(1.0);
    auto collision_corrections_ =
        solver.resolve_collision(collision_info, body_a, body_b);
    EXPECT_TRUE(collision_corrections_.has_value());
    CollisionCorrections ccs = collision_corrections_.value();

    glm::vec3 initial_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float initial_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    body_a.position += ccs.body_a.position;
    body_a.velocity += ccs.body_a.velocity;
    body_a.angular_velocity += ccs.body_a.angular_velocity;

    body_b.position += ccs.body_b.position;
    body_b.velocity += ccs.body_b.velocity;
    body_b.angular_velocity += ccs.body_b.angular_velocity;

    glm::vec3 final_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float final_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    EXPECT_EQ(WorldPoint(20.0f, 0.0f, 0.0f), body_a.position)
        << "body_a.position = " << body_a.position;
    EXPECT_EQ(WorldPoint(-70.0f, 0.0f, 0.0f), body_b.position)
        << "body_b.position = " << body_b.position;
    EXPECT_EQ(glm::vec3(0.0, 0.0, 0.0), body_a.velocity)
        << "body_a.velocity = " << body_a.velocity;
    EXPECT_EQ(glm::vec3(-100.0, 0.0, 0.0), body_b.velocity)
        << "body_b.velocity = " << body_b.velocity;
    EXPECT_EQ(0.0, body_a.angular_velocity);
    EXPECT_EQ(0.0, body_b.angular_velocity);

    EXPECT_EQ(initial_linear_momentum, final_linear_momentum);
    EXPECT_EQ(initial_angular_momentum, final_angular_momentum);
}

TEST(CollisionResolverTest, GivenRectangleCollidesWithStaticObjectExpectBounce) {
    RigidBody body_a = RigidBodyBuilder()
                           .position(WorldPoint(0.0, 45.0, 0.0))
                           .velocity(glm::vec3(0.0, -2.0, 0.0))
                           .shape(Shape::create_rectangle_data(100.0, 100.0))
                           .mass(1.0)
                           .build();
    RigidBody body_b = RigidBodyBuilder()
                           .position(WorldPoint(0.0, -100.0, 0.0))
                           .mass(FLT_MAX)
                           .velocity(glm::vec3(0.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(1000.0, 200.0))
                           .build();

    CollisionInformation collision_info{
        .contact_type = ContactType::EDGE_EDGE,
        .normal = glm::vec3(0.0f, -1.0f, 0.0f),
        .contact_patch = {glm::vec3(-50.0f, -5.0f, 0.0), glm::vec3(50.0f, -5.0f, 0.0)},
        .deepest_contact_idx = 0,
        .penetration_depth = 5.0f};

    auto solver = CollisionSolver(1.0f);
    auto collision_corrections_ =
        solver.resolve_collision(collision_info, body_a, body_b);
    EXPECT_TRUE(collision_corrections_.has_value());
    CollisionCorrections ccs = collision_corrections_.value();

    body_a.position += ccs.body_a.position;
    body_a.velocity += ccs.body_a.velocity;
    body_a.angular_velocity += ccs.body_a.angular_velocity;

    glm::vec3 final_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float final_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    // Note that we do not care about preserving momentum in this test as we don't
    // expect a proper physics response when we collide with a static object
    EXPECT_EQ(WorldPoint(0.0f, 50.0f, 0.0f), body_a.position)
        << "body_a.position = " << body_a.position;
    EXPECT_EQ(glm::vec3(0.0, 2.0, 0.0), body_a.velocity)
        << "body_a.velocity = " << body_a.velocity;
    EXPECT_EQ(0.0, body_a.angular_velocity);
}

// I am not actually sure that I want this feature
TEST(CollisionResolverTest, DISABLED_GivenObjectMoveAwayFromEachOtherExpectNoCollision) {
    RigidBody body_a = RigidBodyBuilder()
                           .position(WorldPoint(-50.0, 0.0, 0.0))
                           .velocity(glm::vec3(-100.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(100.0, 100.0))
                           .build();
    RigidBody body_b = RigidBodyBuilder()
                           .position(WorldPoint(0.0, 0.0, 0.0))
                           .velocity(glm::vec3(100.0, 0.0, 0.0))
                           .shape(Shape::create_rectangle_data(80.0, 80.0))
                           .build();

    CollisionInformation collision_info{
        .penetration_depth = 40.0f,
        .normal = glm::vec3(1.0f, 0.0f, 0.0f),
        .contact_type = ContactType::EDGE_EDGE,
        .contact_patch = {glm::vec3(-40.0f, 40.0, 0.0), glm::vec3(-40.0f, -40.0, 0.0)},
        .deepest_contact_idx = 0,
    };

    auto solver = CollisionSolver(1.0f);
    auto collision_corrections_ =
        solver.resolve_collision(collision_info, body_a, body_b);
    EXPECT_FALSE(collision_corrections_.has_value());
}
