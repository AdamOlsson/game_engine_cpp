#include "Coordinates.h"
#include "io.h"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"
#include "physics_engine/collision_resolver.h"
#include <gtest/gtest.h>

TEST(CollisionResolverTest,
     GivenTwoRectanglesWhenTheyHaveNoVelocityExpectPositionalChange) {

    RigidBody body_a = RigidBody{.position = WorldPoint(-4.0f, 0.0f, 0.0f),
                                 .shape = Shape::create_rectangle_data(10.0f, 10.0f),
                                 .mass = 1.0};
    RigidBody body_b = RigidBody{.position = WorldPoint(4.0f, 0.0f, 0.0f),
                                 .shape = Shape::create_rectangle_data(10.0f, 10.0f),
                                 .mass = 1.0};

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

    body_a.position += ccs.body_a.position;
    body_a.velocity += ccs.body_a.velocity;
    body_a.angular_velocity += ccs.body_a.angular_velocity;

    body_b.position += ccs.body_b.position;
    body_b.velocity += ccs.body_b.velocity;
    body_b.angular_velocity += ccs.body_b.angular_velocity;

    EXPECT_EQ(WorldPoint(-5.0f, 0.0f, 0.0f), body_a.position)
        << "body_a.position = " << body_a.position;
    EXPECT_EQ(WorldPoint(5.0f, 0.0f, 0.0f), body_b.position)
        << "body_b.position = " << body_b.position;
}
