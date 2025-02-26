#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"
#include "physics_engine/collision_resolver.h"
#include "render_engine/RenderBody.h"
#include <cfloat>
#include <functional>
#include <memory>
#include <optional>

void add_initial_entities(EntityComponentStorage &ecs);

void apply_physics(RigidBody &body, float dt) {
    // TODO: Add accelleration
    body.velocity = (body.position - body.prev_position) / dt;
    body.prev_position = body.position;
    body.position = static_cast<WorldPoint>(body.position + body.velocity * dt);
    body.rotation += body.angular_velocity * dt;
}

constexpr RigidBody BOTTOM_BORDER =
    RigidBody{.position = WorldPoint(0.0, -450.0f, 0.0f),
              .prev_position = WorldPoint(0.0, 450.0f, 0.0f),
              .mass = FLT_MAX,
              .inertia = FLT_MAX,
              .shape = Shape{Rectangle{800.0, 100.0}}};

void apply_correction(const Correction &correction, RigidBody &body, float dt) {
    body.position += correction.position;
    body.velocity += correction.velocity;
    body.rotation += correction.angular_velocity * dt;
    body.prev_position = static_cast<WorldPoint>(body.position - body.velocity * dt);
}

// TODO: Implement, preferably make use of another body just outside of the screen but
// don't apply changes to it
// TODO: Why is it not bouncing? Note that there are tests not translated from Rust
void constrain_to_window(RigidBody &body, float dt) {
    std::optional<CollisionInformation> bottom_collision_info =
        SAT::collision_detection(body, BOTTOM_BORDER);

    if (bottom_collision_info.has_value()) {
        std::optional<CollisionCorrections> correction =
            resolve_collision(bottom_collision_info.value(), body, BOTTOM_BORDER);

        if (correction.has_value()) {
            // TODO: The main problem now that when the object eventually meets the static
            //       edge there will arise an edge-edge collision which I interpret as
            //       vertex-edge collision (because I can't identify edge-edge). This
            //       leads to angular velocity changes which should be linear velocity.
            //       See the test GivenRectangleCollideswithStaticObjectExpectBounce for
            //       an example. I need to properly identify edge-edge collisions
            std::cout << bottom_collision_info.value() << std::endl;
            std::cout << "dynamic body: " << body << std::endl;
            std::cout << "dynamic correction: " << correction.value().body_a << std::endl;
            std::cout << "static correction: " << correction.value().body_b << std::endl;
            // TODO: The updates to the rigid body does not seem to be persistent
            // TODO: The linear velocity is way to small
            apply_correction(correction.value().body_a, body, dt);
            /*body.position += -correction.value().body_b.position;*/

            std::cout << "corrected dynamic body: " << body << std::endl;
            std::cout << std::endl;
        }
    }
}

class Example1SpatialSubdivision : public Game {
  public:
    EntityComponentStorage ecs;

    Example1SpatialSubdivision() : ecs(EntityComponentStorage()) {
        add_initial_entities(ecs);
    };

    ~Example1SpatialSubdivision() {};

    void update(float dt) override {

        ecs.apply_fn<RigidBody>(
            [dt](EntityId id, RigidBody &body) { apply_physics(body, dt); });

        // TODO: Make use of Spatial Subdivision
        // TODO: Make use of SAT

        ecs.apply_fn<RigidBody>(
            [dt](EntityId id, RigidBody &body) { constrain_to_window(body, dt); });
    };

    void render(RenderEngine &render_engine) override {
        std::vector<std::reference_wrapper<const RenderBody>> render_bodies = {};

        // TODO: The common values are only copied during the get_component()
        // function, therefore we can't use direct access with RenderBodies
        for (auto it = ecs.begin<RenderBody>(); it != ecs.end<RenderBody>(); it++) {
            render_bodies.push_back(ecs.get_component<RenderBody>(it.id()).value());
        }

        render_engine.render(render_bodies);
    };

    void setup(RenderEngine &render_engine) override {}
};

void add_initial_entities(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(e1,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(-200.0f, 0.0, 0.0))
                                               .velocity(glm::vec3(0.0, -2.0, 0.0))
                                               .mass(1.0)
                                               .rotation(glm::radians(10.0))
                                               .angular_velocity(glm::radians(0.0))
                                               .shape(Shape::create_triangle_data(200.0))
                                               .collision_restitution(0.7f)
                                               .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    /*EntityId e2 = ecs.create_entity();*/
    /*ecs.add_component<RigidBody>(e2, RigidBodyBuilder()*/
    /*                                     .position(WorldPoint(200.0f, 0.0, 0.0))*/
    /*                                     .velocity(glm::vec3(0.0, -1.0, 0.0))*/
    /*                                     .shape(Shape::create_triangle_data(100.0))*/
    /*                                     .collision_restitution(0.3f)*/
    /*                                     .build());*/
    /*ecs.add_component<RenderBody>(*/
    /*    e2, RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build());*/
}

int main() {
    GameEngineConfig config{};
    config.window_width = 800;
    config.window_height = 800;
    config.window_title = "1_collision_detection";

    auto game = std::make_unique<Example1SpatialSubdivision>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);
    game_engine->run();

    return 0;
}
