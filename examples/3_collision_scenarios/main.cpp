#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "glm/trigonometric.hpp"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"
#include "physics_engine/broadphase/SpatialSubdivision.h"
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
void scenario_8(EntityComponentStorage &ecs);
void scenario_9(EntityComponentStorage &ecs);
void scenario_10(EntityComponentStorage &ecs);

void apply_physics(RigidBody &body, float dt) {
    WorldPoint temp_position = body.position;
    body.position = static_cast<WorldPoint>(2.0f * body.position - body.prev_position +
                                            0.5f * body.acceleration * dt * dt);
    body.prev_position = temp_position;
    body.velocity = (body.position - body.prev_position) / dt;
    body.rotation += body.angular_velocity * dt;
}

void apply_correction(const float dt, const Correction &correction, RigidBody &body) {
    body.position += correction.position;
    body.velocity += correction.velocity;
    body.angular_velocity += correction.angular_velocity;
    body.prev_position = WorldPoint(body.position - body.velocity * dt);
}

class Example3CollisionTypes : public Game {
  public:
    EntityComponentStorage ecs;
    CollisionSolver solver;
    SpatialSubdivision broadphase;
    const uint solver_steps = 6;

    std::vector<ScenarioFunction> scenarios;

    size_t scenario_idx = 0;
    bool collision_has_occured = false;
    size_t frame_count = 121;
    const size_t max_frame_count = 120;

    Example3CollisionTypes()
        : ecs(EntityComponentStorage()), solver(CollisionSolver()),
          broadphase(SpatialSubdivision()) {
        scenarios.push_back(&scenario_10);
        scenarios.push_back(&scenario_9);
        scenarios.push_back(&scenario_8);
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

        auto rigid_bodies = ecs.get_component<RigidBody>();
        auto &rigid_bodies_deref = rigid_bodies->get();
        for (size_t i = 0; i < solver_steps; i++) {
            auto collision_candidates =
                broadphase.collision_detection(rigid_bodies_deref);

            run_narrowphase(dt, collision_candidates.pass1, rigid_bodies_deref);
            run_narrowphase(dt, collision_candidates.pass2, rigid_bodies_deref);
            run_narrowphase(dt, collision_candidates.pass3, rigid_bodies_deref);
            run_narrowphase(dt, collision_candidates.pass4, rigid_bodies_deref);

            collision_has_occured =
                collision_candidates.size() > 0 || collision_has_occured;
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

        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        render_engine.render(render_bodies);

        success = render_engine.end_render_pass();
        if (!success) {
            return;
        }
    };

    void setup(RenderEngine &render_engine) override {}

    void run_narrowphase(const float dt, const std::vector<CollisionCandidates> &pass,
                         std::vector<RigidBody> &rigid_bodies) {
        // The outer loop can be run in parellell
        for (CollisionCandidates cc : pass) {
            // Run collision detection between all pairs in cell (narrowphase)
            for (CollisionCandidatePair ccp : cc) {
                auto &body_a = rigid_bodies[std::get<0>(ccp)];
                auto &body_b = rigid_bodies[std::get<1>(ccp)];
                std::optional<CollisionInformation> collision =
                    SAT::collision_detection(body_a, body_b);

                std::optional<CollisionCorrections> corrections;
                if (collision.has_value()) {
                    corrections =
                        solver.resolve_collision(collision.value(), body_a, body_b);
                }
                if (corrections.has_value()) {
                    apply_correction(dt, corrections->body_a, body_a);
                    apply_correction(dt, corrections->body_b, body_b);
                }
            }
        }
    };
};

void scenario_10(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(-250.0f, -200.0f, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .mass(FLT_MAX)
                          .rotation(glm::radians(45.0f))
                          .shape(Shape::create_rectangle_data(500.0f, 40.0f))
                          .build()));
    ecs.add_component<RenderBody>(e1,
                                  std::move(RenderBodyBuilder().color(GREEN).build()));
    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBodyBuilder()
                          .position(WorldPoint(250.0f, -200.0f, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .mass(FLT_MAX)
                          .rotation(glm::radians(-45.0f))
                          .shape(Shape::create_rectangle_data(500.0f, 40.0f))
                          .build()));
    ecs.add_component<RenderBody>(e2,
                                  std::move(RenderBodyBuilder().color(GREEN).build()));

    const int diff = 400;
    const int max = 3;
    for (auto i = 0; i < max; i++) {
        EntityId e3 = ecs.create_entity();
        ecs.add_component<RigidBody>(
            e3,
            std::move(RigidBodyBuilder()
                          .position(WorldPoint(-200.0f + (diff / max) * i, 300.0, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .acceleration(glm::vec3(0.0, -1000.0, 0.0))
                          .mass(1.0)
                          .collision_restitution(0.8)
                          .shape(Shape::create_circle_data(80.0))
                          .build()));
        ecs.add_component<RenderBody>(e3,
                                      std::move(RenderBodyBuilder().color(RED).build()));
    }
}

void scenario_9(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(-250.0f, -200.0f, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .mass(FLT_MAX)
                          .rotation(glm::radians(45.0f))
                          .shape(Shape::create_rectangle_data(300.0f, 40.0f))
                          .build()));
    ecs.add_component<RenderBody>(e1,
                                  std::move(RenderBodyBuilder().color(GREEN).build()));
    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e2, std::move(RigidBodyBuilder()
                          .position(WorldPoint(250.0f, -200.0f, 0.0))
                          .velocity(glm::vec3(0.0, 0.0, 0.0))
                          .mass(FLT_MAX)
                          .rotation(glm::radians(-45.0f))
                          .shape(Shape::create_rectangle_data(300.0f, 40.0f))
                          .build()));
    ecs.add_component<RenderBody>(e2,
                                  std::move(RenderBodyBuilder().color(GREEN).build()));

    EntityId e3 = ecs.create_entity();
    ecs.add_component<RigidBody>(e3,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(-200.0f, 300.0, 0.0))
                                               .velocity(glm::vec3(0.0, 0.0, 0.0))
                                               .acceleration(glm::vec3(0.0, -1000.0, 0.0))
                                               .mass(1.0)
                                               .collision_restitution(0.8)
                                               .shape(Shape::create_circle_data(100.0))
                                               .build()));
    ecs.add_component<RenderBody>(e3, std::move(RenderBodyBuilder().color(RED).build()));
}

void scenario_8(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(e1,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(0.0f, -400.0f, 0.0))
                                               .velocity(glm::vec3(0.0, 0.0, 0.0))
                                               .mass(FLT_MAX)
                                               .shape(Shape::create_circle_data(300.0f))
                                               .build()));
    ecs.add_component<RenderBody>(e1,
                                  std::move(RenderBodyBuilder().color(GREEN).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(0.0f, 300.0, 0.0))
                                               .velocity(glm::vec3(0.0, 0.0, 0.0))
                                               .acceleration(glm::vec3(0.0, -1000.0, 0.0))
                                               .mass(1.0)
                                               .shape(Shape::create_circle_data(150.0))
                                               .build()));
    ecs.add_component<RenderBody>(e2, std::move(RenderBodyBuilder().color(RED).build()));
}
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
    GameEngineConfig config{.window_config =
                                WindowConfig{.dims = WindowDimensions(800, 800),
                                             .title = "3_collision_scenarios"}};

    auto game = std::make_unique<Example3CollisionTypes>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);
    game_engine->run();

    return 0;
}
