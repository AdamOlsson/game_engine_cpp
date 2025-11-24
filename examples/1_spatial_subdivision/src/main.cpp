#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/WorldPoint.h"
#include "game_engine_sdk/entity_component_storage/ComponentStore.h"
#include "game_engine_sdk/entity_component_storage/EntityComponentStorage.h"
#include "game_engine_sdk/physics_engine/RigidBody.h"
#include "game_engine_sdk/physics_engine/SAT.h"
#include "game_engine_sdk/physics_engine/broadphase/SpatialSubdivision.h"
#include "game_engine_sdk/physics_engine/collision_resolver.h"
#include "game_engine_sdk/render_engine/RenderBody.h"
#include "game_engine_sdk/render_engine/resources/ResourceManager.h"
#include "helper_functions.h"
#include <chrono>
#include <functional>
#include <memory>
#include <optional>

// - TODO: I think that some objects disappear due to NaN, fix this

class Example1SpatialSubdivision : public Game {
  public:
    EntityComponentStorage ecs;
    SpatialSubdivision broadphase;
    CollisionSolver solver;
    const size_t solver_steps = 6;
    const size_t num_entities = 300;

    std::vector<RigidBody> non_spawned_rigid_bodies{};
    std::vector<RenderBody> non_spawned_render_bodies{};

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<float>;

    TimePoint start_tick;
    Duration next_fps_log;
    Duration fps_log_delta;
    float render_count = 0.0;
    const float min_fps = 60.0;

    float continue_spawn = true;
    float spawn_clock = 0.0;
    const float spawn_time_s = 0.08;

    RigidBody SPINNER_RIGID_BODY = RigidBodyBuilder()
                                       .position(WorldPoint(0.0, -60.0f, 0.0f))
                                       .angular_velocity(glm::radians(30.0f))
                                       .mass(FLT_MAX)
                                       .shape(Shape::create_rectangle_data(650.0f, 80.0f))
                                       .build();

    RenderBody SPINNER_RENDER_BODY =
        RenderBodyBuilder().color(util::colors::CYAN).build();

    Example1SpatialSubdivision()
        : ecs(EntityComponentStorage()), solver(CollisionSolver(1.0f)),
          broadphase(SpatialSubdivision()), fps_log_delta(2.0) {
        next_fps_log = fps_log_delta;
        create_initial_entities();
    };

    ~Example1SpatialSubdivision() {};

    void update(const float dt) override {
        spawn_clock += dt;
        if (continue_spawn && spawn_clock > spawn_time_s) {
            spawn();
            spawn_clock = 0.0f;
        }

        apply_physics(dt, SPINNER_RIGID_BODY);
        ecs.apply_fn<RigidBody>(
            [dt](EntityId id, RigidBody &body) { apply_physics(dt, body); });

        auto rigid_bodies = ecs.get_component<RigidBody>();
        auto &rigid_bodies_deref = rigid_bodies->get();

        for (size_t i = 0; i < solver_steps; i++) {
            auto collision_candidates =
                broadphase.collision_detection(rigid_bodies_deref);

            run_narrowphase(dt, collision_candidates.pass1, rigid_bodies_deref);
            run_narrowphase(dt, collision_candidates.pass2, rigid_bodies_deref);
            run_narrowphase(dt, collision_candidates.pass3, rigid_bodies_deref);
            run_narrowphase(dt, collision_candidates.pass4, rigid_bodies_deref);
        }

        ecs.apply_fn<RigidBody>([dt, this](EntityId id, RigidBody &body) {
            constrain_to_window(dt, this->solver, body);
        });

        ecs.apply_fn<RigidBody>([dt, this](EntityId id, RigidBody &body) {
            check_collision_with_spinner(dt, this->solver, SPINNER_RIGID_BODY, body);
        });

        TimePoint current_time = Clock::now();
        Duration elapsed = current_time - start_tick;
        if (elapsed > next_fps_log) {
            const float fps = render_count / fps_log_delta.count();
            std::cout << "FPS: " << fps << ", entities: " << ecs.size<RigidBody>()
                      << std::endl;
            next_fps_log += fps_log_delta;
            continue_spawn = continue_spawn && (fps > min_fps);
            render_count = 0.0;
        }
    };

    void render() override { render_count++; };

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {
        register_all_shaders();
        register_all_fonts();
        register_all_images();

        start_tick = Clock::now();
    }

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

    void spawn() {
        if (non_spawned_rigid_bodies.size() <= 0) {
            return;
        }
        EntityId e = ecs.create_entity();
        ecs.add_component<RigidBody>(e, std::move(non_spawned_rigid_bodies[0]));
        ecs.add_component<RenderBody>(e, std::move(non_spawned_render_bodies[0]));
        non_spawned_rigid_bodies.erase(non_spawned_rigid_bodies.begin());
        non_spawned_render_bodies.erase(non_spawned_render_bodies.begin());
    }

    void create_initial_entities() {

        non_spawned_rigid_bodies.reserve(num_entities);
        non_spawned_render_bodies.reserve(num_entities);

        for (size_t i = 0; i < num_entities; i++) {
            non_spawned_rigid_bodies.push_back(
                std::move(RigidBodyBuilder()
                              .position(WorldPoint(-350.0f, 300.0, 0.0))
                              .velocity(glm::vec3(70.0, 0.0, 0.0))
                              .acceleration(glm::vec3(0.0, -1000.0f, 0.0))
                              .mass(1.0)
                              .collision_restitution(0.5)
                              .angular_velocity(glm::radians(0.0))
                              .shape(Shape::create_triangle_data(20.0))
                              .build()));
            non_spawned_render_bodies.push_back(std::move(
                RenderBodyBuilder()
                    .color(COLORS[non_spawned_rigid_bodies.size() % COLORS.size()])
                    .build()));
        }
    }
};

int main() {
    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "1_collision_detection"}};

    auto game = std::make_unique<Example1SpatialSubdivision>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);
    game_engine->run();

    return 0;
}
