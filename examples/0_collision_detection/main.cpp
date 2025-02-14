#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"
#include "physics_engine/collision_resolver.h"
#include "render_engine/RenderBody.h"
#include "render_engine/Window.h"
#include "shape.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

// DONE: Implement SAT
// DONE: Either click inside does not fully work or the rendering does not correspond to
//       the rigidbody (click on triangle does not work properly)
// DONE: Implement EntityComponentStorage
// DONE: Implement SAT example
//        - Add rectangle rendering
// DONE: Implement Verlet Integration in combination with ECS
//        - DONE: Replace the ECS with the new one
//        - DONE: Implement positional updates when dragging an object
//        - DONE: Builder pattern on RigidBody and RenderBody creation
//        - DONE: prev_position member on RigidBody
//
// TODO: Finalize example
//        - DONE: implement drag physics
//        - DONE: Show collision point again
//        - DONE: Rotate selected object through holding R
//        - DONE: Toggle between triangle-triangle, triangle-rectangle and
//                rectangle-rectangle
// TODO: Make pretty github page for example
//      - TODO: Record sample
//      - TODO: Use GIF in readme
//      - TODO: Write a basic text
void state0(EntityComponentStorage &ecs);
void state1(EntityComponentStorage &ecs);
void state2(EntityComponentStorage &ecs);

class Dev : public Game {
  public:
    EntityComponentStorage ecs;

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;
    TimePoint last_tick;

    std::optional<size_t> selected_entity_id;
    WorldPoint selected_entity_cursor_offset;
    WorldPoint camera_center;

    std::optional<CollisionInformation> collision_point;

    // 0 - Triangle Triangle
    // 1 - Triangle Rectangle
    // 2 - Rectangle Rectangle
    uint32_t state = 0;

    Dev()
        : selected_entity_id(std::nullopt),
          selected_entity_cursor_offset(WorldPoint(0.0, 0.0, 0.0)),
          camera_center(WorldPoint(400.0f, 400.0f, 0.0f)), ecs(EntityComponentStorage()) {

        state0(ecs);
        collision_point = std::nullopt;
    };

    ~Dev() {};

    void update(float dt) override {
        const EntityId body_green_id = 0;
        const EntityId body_red_id = 1;

        if (selected_entity_id.has_value()) {
            auto rb = ecs.get_component<RigidBody>(selected_entity_id.value());
            rb->get().rotation += rb->get().angular_velocity * dt;
        }

        auto body_green = ecs.get_component<RigidBody>(body_green_id);
        auto body_red = ecs.get_component<RigidBody>(body_red_id);

        std::optional<CollisionInformation> collision =
            SAT::collision_detection(body_green->get(), body_red->get());

        collision_point = collision;

        std::optional<CollisionCorrections> ccs;
        if (collision.has_value()) {
            ccs =
                resolve_collision(collision.value(), body_green->get(), body_red->get());
        }

        if (ccs.has_value()) {
            // Only update the non-selected object
            if (selected_entity_id.has_value() &&
                selected_entity_id.value() != body_green_id) {
                ecs.update_component<RigidBody>(body_green_id, [ccs, dt](RigidBody &a) {
                    a.position += ccs->body_a.position;
                    a.velocity += ccs->body_a.velocity;
                    a.rotation += ccs->body_a.angular_velocity;
                    a.prev_position = WorldPoint(a.position - a.velocity * dt);
                });
            }

            if (selected_entity_id.has_value() &&
                selected_entity_id.value() != body_red_id) {
                ecs.update_component<RigidBody>(body_red_id, [ccs, dt](RigidBody &b) {
                    b.position += ccs->body_b.position;
                    b.velocity += ccs->body_b.velocity;
                    b.rotation += ccs->body_b.angular_velocity;
                    b.prev_position = WorldPoint(b.position - b.velocity * dt);
                });
            }
        }
    };

    void render(RenderEngine &render_engine) override {
        std::vector<std::reference_wrapper<const RenderBody>> render_bodies = {};

        // TODO: The common values are only copied during the get_component() function,
        // therefore we can't use direct access with RenderBodies
        for (auto it = ecs.begin<RenderBody>(); it != ecs.end<RenderBody>(); it++) {
            render_bodies.push_back(ecs.get_component<RenderBody>(it.id()).value());
        }

        if (collision_point.has_value()) {
            RenderBody r = RenderBody{.color = glm::vec3(1.0, 1.0, 0.0),
                                      .position = collision_point->collision_point,
                                      .shape = Shape::create_rectangle_data(10.0, 10.0)};
            render_bodies.push_back(std::cref(r));
        }

        render_engine.render(render_bodies);
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this](MouseEvent e, ViewportPoint &p) { this->handle_mouse_event(e, p); });
        render_engine.register_keyboard_event_callback(
            [this](KeyEvent &k, KeyState &s) { this->handle_keyboard_event(k, s); });
    }

    void handle_mouse_event(MouseEvent event, ViewportPoint &point) {
        switch (event) {
        case MouseEvent::LEFT_BUTTON_UP:
            if (selected_entity_id.has_value()) {
                WorldPoint wp = point.to_world_point(camera_center);
                size_t entity_handle = selected_entity_id.value();
                ecs.update_component<RigidBody>(
                    entity_handle, [this, entity_handle, wp](RigidBody &r) {
                        r.prev_position = r.position;
                        r.position.x = wp.x + selected_entity_cursor_offset.x;
                        r.position.y = wp.y + selected_entity_cursor_offset.y;
                        r.velocity = r.position - r.prev_position;
                        r.angular_velocity = 0.0;
                    });
            }
            selected_entity_id = std::nullopt;
            selected_entity_cursor_offset.x = 0.0;
            selected_entity_cursor_offset.y = 0.0;
            selected_entity_cursor_offset.z = 0.0;
            break;
        case MouseEvent::LEFT_BUTTON_DOWN: {
            WorldPoint wp = point.to_world_point(camera_center);

            for (auto it = ecs.begin<RigidBody>(); it != ecs.end<RigidBody>(); it++) {
                RigidBody &body = *it;
                if (body.is_point_inside(wp)) {
                    selected_entity_id = it.id();
                    selected_entity_cursor_offset.x = body.position.x - wp.x;
                    selected_entity_cursor_offset.y = body.position.y - wp.y;
                    break;
                }
            }
        } break;
        case MouseEvent::RIGHT_BUTTON_DOWN:
            break;
        case MouseEvent::RIGHT_BUTTON_UP:
            break;
        case MouseEvent::CURSOR_MOVED:
            if (selected_entity_id.has_value()) {
                WorldPoint wp = point.to_world_point(camera_center);
                size_t entity_handle = selected_entity_id.value();
                ecs.update_component<RigidBody>(
                    entity_handle, [this, entity_handle, wp](RigidBody &r) {
                        r.prev_position = r.position;
                        r.position.x = wp.x + selected_entity_cursor_offset.x;
                        r.position.y = wp.y + selected_entity_cursor_offset.y;
                        r.velocity = r.position - r.prev_position;
                    });
            }
            break;
        default:
            break;
        }
    }

    void handle_keyboard_event(KeyEvent &key, KeyState &key_state) {
        switch (key) {
        case KeyEvent::T: {
            if (key_state != KeyState::DOWN) {
                return;
            }
            state = (state + 1) % 3;
            selected_entity_id = std::nullopt;
            selected_entity_cursor_offset.x = 0.0;
            selected_entity_cursor_offset.y = 0.0;
            selected_entity_cursor_offset.z = 0.0;
            ecs = EntityComponentStorage();
            if (state == 0) {
                state0(ecs);
            } else if (state == 1) {
                state1(ecs);

            } else {
                state2(ecs);
            }
            break;
        }
        case KeyEvent::R: {
            if (!selected_entity_id.has_value()) {
                return;
            }

            if (key_state == KeyState::DOWN) {
                ecs.update_component<RigidBody>(
                    selected_entity_id.value(),
                    [](RigidBody &r) { r.angular_velocity = glm::radians(45.0); });

            } else if (key_state == KeyState::UP) {
                ecs.update_component<RigidBody>(
                    selected_entity_id.value(),
                    [](RigidBody &r) { r.angular_velocity = 0.0; });
            }
            break;
        }
        }
    };
};

void state0(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(e1,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(-200.0f, 0.0, 0.0))
                                               .shape(Shape::create_triangle_data(200.0))
                                               .collision_restitution(0.3f)
                                               .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2, RigidBodyBuilder()
                                         .position(WorldPoint(200.0f, 0.0, 0.0))
                                         .shape(Shape::create_triangle_data(200.0))
                                         .collision_restitution(0.3f)
                                         .build());
    ecs.add_component<RenderBody>(
        e2, RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build());
}

void state1(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(e1,
                                 std::move(RigidBodyBuilder()
                                               .position(WorldPoint(-200.0f, 0.0, 0.0))
                                               .shape(Shape::create_triangle_data(200.0))
                                               .collision_restitution(0.3f)
                                               .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 RigidBodyBuilder()
                                     .position(WorldPoint(200.0f, 0.0, 0.0))
                                     .shape(Shape::create_rectangle_data(180.0, 180.0))
                                     .collision_restitution(0.3f)
                                     .build());
    ecs.add_component<RenderBody>(
        e2, RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build());
}

void state2(EntityComponentStorage &ecs) {
    EntityId e1 = ecs.create_entity();
    ecs.add_component<RigidBody>(
        e1, std::move(RigidBodyBuilder()
                          .position(WorldPoint(-200.0f, 0.0, 0.0))
                          .shape(Shape::create_rectangle_data(180.0, 180.0))
                          .collision_restitution(0.3f)
                          .build()));
    ecs.add_component<RenderBody>(
        e1, std::move(RenderBodyBuilder().color(glm::vec3(0.0, 1.0, 0.0)).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 RigidBodyBuilder()
                                     .position(WorldPoint(200.0f, 0.0, 0.0))
                                     .shape(Shape::create_rectangle_data(180.0, 180.0))
                                     .collision_restitution(0.3f)
                                     .build());
    ecs.add_component<RenderBody>(
        e2, RenderBodyBuilder().color(glm::vec3(1.0, 0.0, 0.0)).build());
}

int main() {
    GameEngineConfig config{};
    config.window_width = 800;
    config.window_height = 800;
    config.window_title = "0_collision_detection";

    auto game = std::make_unique<Dev>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);
    std::cout << std::endl << std::endl;
    std::cout << "Control:" << std::endl;
    std::cout << "  Left Mouse - move selected shape around" << std::endl;
    std::cout << "  T          - to toggle between shapes" << std::endl;
    std::cout << "  R          - hold to rotate selected shape (while holding Left Mouse)"
              << std::endl;
    std::cout << std::endl << std::endl;

    game_engine->run();

    return 0;
}
