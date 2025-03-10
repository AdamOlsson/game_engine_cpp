#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"
#include "physics_engine/collision_resolver.h"
#include "render_engine/RenderBody.h"
#include <functional>
#include <memory>
#include <optional>

constexpr glm::vec3 RED = glm::vec3(0.5f, 0.0, 0.0);
constexpr glm::vec3 GREEN = glm::vec3(0.0, 0.5f, 0.0);
using ScenarioFunction = void (*)(EntityComponentStorage &ecs);
void scenario_0(EntityComponentStorage &ecs);
void scenario_1(EntityComponentStorage &ecs);
void scenario_2(EntityComponentStorage &ecs);
void scenario_3(EntityComponentStorage &ecs);
void scenario_4(EntityComponentStorage &ecs);
void scenario_5(EntityComponentStorage &ecs);
void scenario_6(EntityComponentStorage &ecs);
void scenario_7(EntityComponentStorage &ecs);

void apply_physics(RigidBody &body, float dt) {
    WorldPoint temp_position = body.position;
    body.position = static_cast<WorldPoint>(2.0f * body.position - body.prev_position +
                                            0.5f * body.acceleration * dt * dt);
    body.prev_position = temp_position;
    body.velocity = (body.position - body.prev_position) / dt;
    body.rotation += body.angular_velocity * dt;
}

class Example3CollisionTypes : public Game {
  public:
    EntityComponentStorage ecs;
    CollisionSolver solver;

    std::vector<ScenarioFunction> scenarios;

    size_t scenario_idx = 0;
    bool collision_has_occured = false;
    size_t frame_count = 121;
    const size_t max_frame_count = 120;

    Example3CollisionTypes() : ecs(EntityComponentStorage()), solver(CollisionSolver()) {
        scenarios.push_back(&scenario_7);
        scenarios.push_back(&scenario_6);
        scenarios.push_back(&scenario_5);
        scenarios.push_back(&scenario_4);
        scenarios.push_back(&scenario_3);
        scenarios.push_back(&scenario_2);
        scenarios.push_back(&scenario_1);
        scenarios.push_back(&scenario_0);
    };

    ~Example3CollisionTypes() {};

    void update(float dt) override {
        if (frame_count > max_frame_count) {
            ecs = EntityComponentStorage(); // reset
            scenarios[scenario_idx](ecs);
            collision_has_occured = false;
            frame_count = 0;
            scenario_idx = (scenario_idx + 1) % scenarios.size();
        }

        ecs.apply_fn<RigidBody>(
            [dt](EntityId id, RigidBody &body) { apply_physics(body, dt); });

        auto body_0 = ecs.get_component<RigidBody>(0);
        auto body_1 = ecs.get_component<RigidBody>(1);
        std::optional<CollisionInformation> collision =
            SAT::collision_detection(body_0->get(), body_1->get());

        std::optional<CollisionCorrections> ccs;
        if (collision.has_value()) {
            collision_has_occured = true;
            ccs =
                solver.resolve_collision(collision.value(), body_0->get(), body_1->get());
        }

        if (ccs.has_value()) {
            ecs.update_component<RigidBody>(0, [ccs, dt](RigidBody &a) {
                a.position += ccs->body_a.position;
                a.velocity += ccs->body_a.velocity;
                a.angular_velocity += ccs->body_a.angular_velocity;
                a.prev_position = WorldPoint(a.position - a.velocity * dt);
            });
            ecs.update_component<RigidBody>(1, [ccs, dt](RigidBody &b) {
                b.position += ccs->body_b.position;
                b.velocity += ccs->body_b.velocity;
                b.angular_velocity += ccs->body_b.angular_velocity;
                b.prev_position = WorldPoint(b.position - b.velocity * dt);
            });
        }

        if (collision_has_occured) {
            frame_count++;
        }
    };

    void render(RenderEngine &render_engine) override {
        std::vector<std::reference_wrapper<const RenderBody>> render_bodies = {};

        for (auto it = ecs.begin<RenderBody>(); it != ecs.end<RenderBody>(); it++) {
            auto &render_body = *it;
            auto rigid_body = ecs.get_component<RigidBody>(it.id()).value();
            render_body.position = rigid_body.get().position;
            render_body.rotation = rigid_body.get().rotation;
            render_body.shape = rigid_body.get().shape;
            render_bodies.push_back(render_body);
        }

        render_engine.render(render_bodies);
    };

    void setup(RenderEngine &render_engine) override {}
};

void scenario_7(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(0.0f, -400.0f, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .mass(FLT_MAX)
                          .shape(Shape::create_rectangle_data(800.0f, 100.0f))
                          .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(0.0f, 300.0, 0.0))
                                               .velocity(glm::vec3(0.0, 0.0, 0.0))
                                               .acceleration(glm::vec3(0.0, -1000.0, 0.0))
                                               .mass(1.0)
                                               .angular_velocity(glm::radians(20.0))
                                               .shape(Shape::create_triangle_data(150.0))
                                               .build()));
    ecs.add_component<RenderBody>(
        e2, std::move(RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build()));
}

void scenario_6(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(e1,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(0.0f, -400.0f, 0.0))
                                               .velocity(glm::vec3(0.0, 0.0, 0.0))
                                               .mass(FLT_MAX)
                                               .angular_velocity(glm::radians(30.0f))
                                               .shape(Shape::create_triangle_data(400.0))
                                               .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(100.0f, 100.0, 0.0))
                                               .velocity(glm::vec3(0.0, 0.0, 0.0))
                                               .acceleration(glm::vec3(0.0, -1000.0, 0.0))
                                               .mass(1.0)
                                               .angular_velocity(glm::radians(0.0))
                                               .shape(Shape::create_triangle_data(150.0))
                                               .build()));
    ecs.add_component<RenderBody>(
        e2, std::move(RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build()));
}

void scenario_5(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(e1,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(0.0f, -200.0f, 0.0))
                                               .velocity(glm::vec3(0.0, 5.0, 0.0))
                                               .mass(1.0)
                                               .rotation(glm::radians(180.0f))
                                               .angular_velocity(glm::radians(0.0))
                                               .shape(Shape::create_triangle_data(150.0))
                                               .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(100.0f, 100.0, 0.0))
                                               .velocity(glm::vec3(0.0, -5.0, 0.0))
                                               .mass(1.0)
                                               .angular_velocity(glm::radians(0.0))
                                               .shape(Shape::create_triangle_data(150.0))
                                               .build()));
    ecs.add_component<RenderBody>(
        e2, std::move(RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build()));
}

void scenario_4(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(0.0f, -200.0f, 0.0))
                          .velocity(glm::vec3(0.0, 5.0, 0.0))
                          .mass(1.0)
                          .angular_velocity(glm::radians(0.0))
                          .shape(Shape::create_rectangle_data(300.0, 50.0))
                          .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(100.0f, 100.0, 0.0))
                                               .velocity(glm::vec3(0.0, -5.0, 0.0))
                                               .mass(1.0)
                                               .angular_velocity(glm::radians(0.0))
                                               .shape(Shape::create_triangle_data(150.0))
                                               .build()));
    ecs.add_component<RenderBody>(
        e2, std::move(RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build()));
}

void scenario_3(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(0.0f, 200.0f, 0.0))
                          .velocity(glm::vec3(0.0, -5.0, 0.0))
                          .mass(1.0)
                          .angular_velocity(glm::radians(0.0))
                          .shape(Shape::create_rectangle_data(300.0, 50.0))
                          .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(100.0f, -100.0, 0.0))
                                               .velocity(glm::vec3(0.0, 5.0, 0.0))
                                               .mass(1.0)
                                               .angular_velocity(glm::radians(0.0))
                                               .shape(Shape::create_triangle_data(150.0))
                                               .build()));
    ecs.add_component<RenderBody>(
        e2, std::move(RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build()));
}

void scenario_2(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(-200.0f, 0.0, 0.0))
                          .velocity(glm::vec3(10.0, 0.0, 0.0))
                          .mass(1.0)
                          .angular_velocity(glm::radians(0.0))
                          .shape(Shape::create_rectangle_data(100.0, 100.0))
                          .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBodyBuilder()
                          .position(WorldPoint(50.0f, -100.0, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .mass(1.0)
                          .angular_velocity(glm::radians(0.0))
                          .shape(Shape::create_rectangle_data(50.0, 200.0))
                          .build()));
    ecs.add_component<RenderBody>(
        e2, std::move(RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build()));
}

void scenario_1(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(-200.0f, 0.0, 0.0))
                          .velocity(glm::vec3(10.0, 0.0, 0.0))
                          .mass(1.0)
                          .angular_velocity(glm::radians(0.0))
                          .shape(Shape::create_rectangle_data(100.0, 100.0))
                          .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBodyBuilder()
                          .position(WorldPoint(500.0f, 0.0, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .mass(FLT_MAX)
                          .angular_velocity(glm::radians(0.0))
                          .shape(Shape::create_rectangle_data(1000.0, 1000.0))
                          .build()));
    ecs.add_component<RenderBody>(
        e2, std::move(RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build()));
}

void scenario_0(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(-200.0f, 0.0, 0.0))
                          .velocity(glm::vec3(10.0, 0.0, 0.0))
                          .mass(1.0)
                          .angular_velocity(glm::radians(0.0))
                          .shape(Shape::create_rectangle_data(100.0, 100.0))
                          /*.collision_restitution(0.7f)*/
                          .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBodyBuilder()
                          .position(WorldPoint(50.0f, 0.0, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .mass(1.0)
                          .angular_velocity(glm::radians(0.0))
                          .shape(Shape::create_rectangle_data(100.0, 100.0))
                          .build()));
    ecs.add_component<RenderBody>(
        e2, std::move(RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build()));
}

int main() {
    GameEngineConfig config{};
    config.window_width = 800;
    config.window_height = 800;
    config.window_title = "3_collision_scenarios";

    auto game = std::make_unique<Example3CollisionTypes>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);
    game_engine->run();

    return 0;
}
