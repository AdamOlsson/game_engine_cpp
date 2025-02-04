#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "physics_engine/RigidBody.h"
#include <gtest/gtest.h>

TEST(ECSTest, TestAdd) {
    EntityComponentStorage ecs = EntityComponentStorage();
    EntityId e = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                               .shape = Shape::create_triangle_data(3.0)}));
    auto r1 = ecs.get_component<RigidBody>(e);
    EXPECT_TRUE(r1.has_value());
    EXPECT_EQ(WorldPoint(10.0, 0.0, 0.0), r1->get().position);
    EXPECT_EQ(3.0, r1->get().shape.get<Triangle>().side);
}

TEST(ECSTest, TestAddTwoObjects) {
    EntityComponentStorage ecs = EntityComponentStorage();
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                                .shape = Shape::create_triangle_data(3.0)}));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBody{.position = WorldPoint(200.0, 0.0, 0.0),
                                .shape = Shape::create_rectangle_data(2.0, 8.0)}));

    auto r1 = ecs.get_component<RigidBody>(e1);
    auto r2 = ecs.get_component<RigidBody>(e2);
    EXPECT_TRUE(r1.has_value());
    EXPECT_TRUE(r2.has_value());
    EXPECT_EQ(WorldPoint(10.0, 0.0, 0.0), r1->get().position);
    EXPECT_EQ(WorldPoint(200.0, 0.0, 0.0), r2->get().position);
    EXPECT_EQ(3.0, r1->get().shape.get<Triangle>().side);
    EXPECT_EQ(2.0, r2->get().shape.get<Rectangle>().width);
    EXPECT_EQ(8.0, r2->get().shape.get<Rectangle>().height);
}

TEST(ECSTest, TestUpdateFromReference) {
    EntityComponentStorage ecs = EntityComponentStorage();
    EntityId e = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                               .shape = Shape::create_triangle_data(3.0)}));
    auto r0 = ecs.get_component<RigidBody>(e);
    EXPECT_EQ(WorldPoint(10.0, 0.0, 0.0), r0->get().position);
    EXPECT_EQ(3.0, r0->get().shape.get<Triangle>().side);

    auto r1_ = ecs.get_component<RigidBody>(e);
    EXPECT_TRUE(r1_.has_value());
    auto r1 = r1_.value();
    r1.get().position = WorldPoint(0.0, 20.0, 0.0);
    r1.get().shape = Shape::create_rectangle_data(20.0, 30.0);

    auto r2 = ecs.get_component<RigidBody>(e);
    EXPECT_EQ(WorldPoint(0.0, 20.0, 0.0), r2->get().position);
    EXPECT_EQ(20.0, r2->get().shape.get<Rectangle>().width);
    EXPECT_EQ(30.0, r2->get().shape.get<Rectangle>().height);
}

TEST(ECSTest, TestUpdateTwoObjectsFromReference) {
    EntityComponentStorage ecs = EntityComponentStorage();
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                                .shape = Shape::create_triangle_data(3.0)}));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBody{.position = WorldPoint(200.0, 0.0, 0.0),
                                .shape = Shape::create_rectangle_data(2.0, 8.0)}));

    auto r1_ = ecs.get_component<RigidBody>(e1);
    auto r2_ = ecs.get_component<RigidBody>(e2);
    r1_->get().position = WorldPoint(0.0, 20.0, 0.0);
    r2_->get().position = WorldPoint(0.0, 30.0, 0.0);

    auto r1 = ecs.get_component<RigidBody>(e1);
    auto r2 = ecs.get_component<RigidBody>(e2);
    EXPECT_TRUE(r1.has_value());
    EXPECT_TRUE(r2.has_value());
    EXPECT_EQ(WorldPoint(0.0, 20.0, 0.0), r1->get().position);
    EXPECT_EQ(WorldPoint(0.0, 30.0, 0.0), r2->get().position);
    EXPECT_EQ(3.0, r1->get().shape.get<Triangle>().side);
    EXPECT_EQ(2.0, r2->get().shape.get<Rectangle>().width);
    EXPECT_EQ(8.0, r2->get().shape.get<Rectangle>().height);
}

TEST(ECSTest, TestUpdateFromCallbackUsingLambda) {
    EntityComponentStorage ecs = EntityComponentStorage();
    EntityId e = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                               .shape = Shape::create_triangle_data(3.0)}));

    ecs.update_component<RigidBody>(e, [](RigidBody &r) {
        r.position = WorldPoint(0.0, 20.0, 0.0);
        r.shape = Shape::create_rectangle_data(20.0, 30.0);
    });

    auto r2 = ecs.get_component<RigidBody>(e);
    EXPECT_EQ(WorldPoint(0.0, 20.0, 0.0), r2->get().position);
    EXPECT_EQ(20.0, r2->get().shape.get<Rectangle>().width);
    EXPECT_EQ(30.0, r2->get().shape.get<Rectangle>().height);
}

void update_rigid_body(RigidBody &r) {
    r.position = WorldPoint(0.0, 20.0, 0.0);
    r.shape = Shape::create_rectangle_data(20.0, 30.0);
}

TEST(ECSTest, TestUpdateFromCallbackUsingFunction) {
    EntityComponentStorage ecs = EntityComponentStorage();
    EntityId e = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                               .shape = Shape::create_triangle_data(3.0)}));

    ecs.update_component<RigidBody>(e, update_rigid_body);

    auto r2 = ecs.get_component<RigidBody>(e);
    EXPECT_EQ(WorldPoint(0.0, 20.0, 0.0), r2->get().position);
    EXPECT_EQ(20.0, r2->get().shape.get<Rectangle>().width);
    EXPECT_EQ(30.0, r2->get().shape.get<Rectangle>().height);
}

TEST(ECSTest, TestIteratorsWhenEmpty) {
    EntityComponentStorage ecs = EntityComponentStorage();

    for (auto it = ecs.begin<RigidBody>(); it != ecs.end<RigidBody>(); it++) {
        EXPECT_TRUE(false); // Fail if this line is run
    }

    for (auto it = ecs.begin<RigidBody>(); it != ecs.end<RigidBody>(); ++it) {
        EXPECT_TRUE(false); // Fail if this line is run
    }

    for (auto comp : ecs.view<RigidBody>()) {
        EXPECT_TRUE(false); // Fail if this line is run
    }

    EXPECT_TRUE(true);
}

TEST(ECSTest, TestIteratoringOverAllValues) {
    EntityComponentStorage ecs = EntityComponentStorage();
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                                .shape = Shape::create_triangle_data(3.0)}));
    EXPECT_EQ(1, ecs.size<RigidBody>());
    EXPECT_EQ(0, ecs.size<RenderBody>());

    int count1 = 0;
    for (auto it = ecs.begin<RigidBody>(); it != ecs.end<RigidBody>(); it++) {
        count1++;
    }
    EXPECT_EQ(1, count1);

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                                .shape = Shape::create_triangle_data(3.0)}));
    EXPECT_EQ(2, ecs.size<RigidBody>());
    EXPECT_EQ(0, ecs.size<RenderBody>());

    int count2 = 0;
    for (auto it = ecs.begin<RigidBody>(); it != ecs.end<RigidBody>(); ++it) {
        count2++;
    }
    EXPECT_EQ(2, count2);

    EntityId e3 = ecs.create_entity();
    ecs.add_component<RenderBody>(
        e3, std::move(RenderBody{.color = glm::vec3(1.0, 1.0, 1.0)}));
    EXPECT_EQ(2, ecs.size<RigidBody>());
    EXPECT_EQ(1, ecs.size<RenderBody>());

    int count3 = 0;
    for (auto comp : ecs.view<RenderBody>()) {
        count3++;
    }
    EXPECT_EQ(1, count3);
}

TEST(ECSTest, TestIteratingOverComponentAndPerformUpdates) {
    EntityComponentStorage ecs = EntityComponentStorage();
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBody{.position = WorldPoint(10.0, 0.0, 0.0),
                                .shape = Shape::create_triangle_data(3.0)}));

    for (auto it = ecs.begin<RigidBody>(); it != ecs.end<RigidBody>(); it++) {
        EntityId id = it.id();
        RigidBody &r = *it;
        r.position = WorldPoint(0.0, 20.0, 0.0);
        r.shape = Shape::create_rectangle_data(20.0, 30.0);
    }

    auto r = ecs.get_component<RigidBody>(e1);
    EXPECT_EQ(WorldPoint(0.0, 20.0, 0.0), r->get().position);
    EXPECT_EQ(20.0, r->get().shape.get<Rectangle>().width);
    EXPECT_EQ(30.0, r->get().shape.get<Rectangle>().height);

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBody{.position = WorldPoint(100.0, 0.0, 0.0),
                                .shape = Shape::create_triangle_data(30.0)}));

    for (auto it = ecs.begin<RigidBody>(); it != ecs.end<RigidBody>(); it++) {
        if (it.id() == e1) {
            continue;
        }
        RigidBody &r = *it;
        r.position = WorldPoint(0.0, 200.0, 0.0);
        r.shape = Shape::create_rectangle_data(220.0, 330.0);
    }

    auto r1 = ecs.get_component<RigidBody>(e1);
    EXPECT_EQ(WorldPoint(0.0, 20.0, 0.0), r1->get().position);
    EXPECT_EQ(20.0, r1->get().shape.get<Rectangle>().width);
    EXPECT_EQ(30.0, r1->get().shape.get<Rectangle>().height);

    auto r2 = ecs.get_component<RigidBody>(e2);
    EXPECT_EQ(WorldPoint(0.0, 200.0, 0.0), r2->get().position);
    EXPECT_EQ(220.0, r2->get().shape.get<Rectangle>().width);
    EXPECT_EQ(330.0, r2->get().shape.get<Rectangle>().height);
}
