#include "Coordinates.h"
#include "io.h"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"
#include "physics_engine/collision_resolver.h"
#include "test_utils.h"
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
