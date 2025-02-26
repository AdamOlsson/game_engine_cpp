#include "Coordinates.h"
#include "equations/equations.h"
#include "io.h"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"
#include "physics_engine/collision_resolver.h"
#include <cfloat>
#include <gtest/gtest.h>

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

    std::optional<CollisionInformation> collision_info_ =
        SAT::collision_detection(body_a, body_b);

    EXPECT_TRUE(collision_info_.has_value());

    CollisionInformation collision_info = collision_info_.value();

    EXPECT_EQ(glm::vec3(1.0f, 0.0f, 0.0f), collision_info.normal);
    EXPECT_EQ(2.0f, collision_info.penetration_depth);
    EXPECT_EQ(glm::vec3(-1.0f, 5.0f, 0.0f), collision_info.collision_point);

    auto collision_corrections_ = resolve_collision(collision_info, body_a, body_b);
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

    EXPECT_EQ(WorldPoint(-5.0f, 0.0f, 0.0f), body_a.position)
        << "body_a.position = " << body_a.position;
    EXPECT_EQ(WorldPoint(5.0f, 0.0f, 0.0f), body_b.position)
        << "body_b.position = " << body_b.position;

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

    EXPECT_EQ(glm::vec3(1.0f, 0.0f, 0.0f), collision_info.normal);
    EXPECT_EQ(40.0f, collision_info.penetration_depth);
    EXPECT_EQ(glm::vec3(-40.0f, -25.0f, 0.0f), collision_info.collision_point)
        << "collision_info.collision_point = " << collision_info.collision_point;

    auto collision_corrections_ = resolve_collision(collision_info, body_a, body_b);
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

    CollisionInformation collision_info{.normal = glm::vec3(1.0f, 0.0f, 0.0f),
                                        .collision_point = glm::vec3(-40.0f, 0.0, 0.0),
                                        .penetration_depth = 40.0f};

    auto collision_corrections_ = resolve_collision(collision_info, body_a, body_b);
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

    CollisionInformation collision_info{.normal = glm::vec3(1.0f, 0.0f, 0.0f),
                                        .collision_point = glm::vec3(-40.0f, 0.0, 0.0),
                                        .penetration_depth = 40.0f};

    auto collision_corrections_ = resolve_collision(collision_info, body_a, body_b);
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

TEST(CollisionResolverTest, GivenRectangleCollideswithStaticObjectExpectBounce) {
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

    CollisionInformation collision_info{.normal = glm::vec3(0.0f, -1.0f, 0.0f),
                                        .collision_point = glm::vec3(0.0f, -5.0, 0.0),
                                        .penetration_depth = 5.0f};

    auto collision_corrections_ = resolve_collision(collision_info, body_a, body_b);
    EXPECT_TRUE(collision_corrections_.has_value());
    CollisionCorrections ccs = collision_corrections_.value();

    glm::vec3 initial_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float initial_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    body_a.position += ccs.body_a.position;
    body_a.velocity += ccs.body_a.velocity;
    body_a.angular_velocity += ccs.body_a.angular_velocity;

    // In a scenario where this is static, we don't apply these changes to the static body
    // this also as the effect that we break law of perservation of momentum
    body_b.position += ccs.body_b.position;
    body_b.velocity += ccs.body_b.velocity;
    body_b.angular_velocity += ccs.body_b.angular_velocity;

    /*std::cout << "A: " << ccs.body_a << std::endl;*/
    /*std::cout << "B: " << ccs.body_b << std::endl;*/

    glm::vec3 final_linear_momentum =
        Equations::linear_momentum(body_a) + Equations::linear_momentum(body_b);
    float final_angular_momentum =
        Equations::angular_momentum(body_a) + Equations::angular_momentum(body_b);

    EXPECT_EQ(WorldPoint(0.0f, 50.0f, 0.0f), body_a.position)
        << "body_a.position = " << body_a.position;
    EXPECT_EQ(glm::vec3(0.0, 2.0, 0.0), body_a.velocity)
        << "body_a.velocity = " << body_a.velocity;
    EXPECT_EQ(0.0, body_a.angular_velocity);

    EXPECT_EQ(initial_linear_momentum, final_linear_momentum)
        << "Expected " << initial_linear_momentum << " but found "
        << final_linear_momentum;
    EXPECT_EQ(initial_angular_momentum, final_angular_momentum)
        << "Expected " << initial_angular_momentum << " but found "
        << final_angular_momentum;
}

TEST(CollisionResolverTest, GivenObjectMoveAwayFromEachOtherExpectNoCollision) {
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

    CollisionInformation collision_info{.normal = glm::vec3(1.0f, 0.0f, 0.0f),
                                        .collision_point = glm::vec3(-40.0f, 0.0, 0.0),
                                        .penetration_depth = 40.0f};

    auto collision_corrections_ = resolve_collision(collision_info, body_a, body_b);
    EXPECT_FALSE(collision_corrections_.has_value());
}
