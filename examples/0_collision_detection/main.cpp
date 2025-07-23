#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "glm/geometric.hpp"
#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"
#include "physics_engine/collision_resolver.h"
#include "render_engine/RenderBody.h"
#include "render_engine/colors.h"
#include "shape.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

constexpr glm::vec3 RED = glm::vec3(0.5f, 0.0, 0.0);
constexpr glm::vec3 GREEN = glm::vec3(0.0, 0.5f, 0.0);
constexpr glm::vec3 YELLOW = glm::vec3(0.5f, 0.5f, 0.0);

void state0(EntityComponentStorage &ecs);
void state1(EntityComponentStorage &ecs);
void state2(EntityComponentStorage &ecs);

class Example0CollisionDetection : public Game {
  public:
    EntityComponentStorage ecs;

    std::optional<size_t> selected_entity_id;
    WorldPoint selected_entity_cursor_offset;
    WorldPoint camera_center;
    CollisionSolver solver;

    std::optional<CollisionInformation> collision_point;

    // 0 - Triangle Triangle
    // 1 - Triangle Rectangle
    // 2 - Rectangle Rectangle
    uint32_t state = 0;

    Example0CollisionDetection()
        : selected_entity_id(std::nullopt),
          selected_entity_cursor_offset(WorldPoint(0.0, 0.0, 0.0)),
          camera_center(WorldPoint(400.0f, 400.0f, 0.0f)), ecs(EntityComponentStorage()),
          solver(CollisionSolver()) {

        state0(ecs);
        collision_point = std::nullopt;
    };

    ~Example0CollisionDetection() {};

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
            ccs = solver.resolve_collision(collision.value(), body_green->get(),
                                           body_red->get());
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

        for (auto it = ecs.begin<RenderBody>(); it != ecs.end<RenderBody>(); it++) {
            auto &render_body = *it;
            auto rigid_body = ecs.get_component<RigidBody>(it.id()).value();
            render_body.position = rigid_body.get().position;
            render_body.rotation = rigid_body.get().rotation;
            render_body.shape = rigid_body.get().shape;
            render_bodies.push_back(render_body);
        }

        if (collision_point.has_value()) {
            switch (collision_point->contact_type) {
            case ContactType::NONE:
                break;
            case ContactType::VERTEX_VERTEX:
            case ContactType::VERTEX_EDGE: {
                const RenderBody r = RenderBody{
                    .color = colors::YELLOW,
                    .position = collision_point
                                    ->contact_patch[collision_point->deepest_contact_idx],
                    .shape = Shape::create_circle_data(10.0)};
                render_bodies.push_back(std::cref(r));
                break;
            }
            case ContactType::EDGE_EDGE: {
                const auto line =
                    collision_point->contact_patch[1] - collision_point->contact_patch[0];
                const auto center = collision_point->contact_patch[0] + (line / 2.0f);
                const float length = glm::length(line);
                const float thickness = 10.0f;
                const float rotation =
                    std::atan2(line.y, line.x); // Crazy expensive operation
                const RenderBody r =
                    RenderBody{.color = colors::YELLOW,
                               .position = center,
                               .rotation = -rotation, // glm has opposite hand system
                               .shape = Shape::create_rectangle_data(length, thickness)};
                render_bodies.push_back(std::cref(r));
                break;
            }
            };
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
        collision_point = std::nullopt;
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this](window::MouseEvent e, window::ViewportPoint &p) {
                this->handle_mouse_event(e, p);
            });
        render_engine.register_keyboard_event_callback(
            [this](window::KeyEvent &k, window::KeyState &s) {
                this->handle_keyboard_event(k, s);
            });
    }

    void handle_mouse_event(window::MouseEvent event, window::ViewportPoint &point) {
        switch (event) {
        case window::MouseEvent::LEFT_BUTTON_UP:
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
        case window::MouseEvent::LEFT_BUTTON_DOWN: {
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
        case window::MouseEvent::RIGHT_BUTTON_DOWN:
            break;
        case window::MouseEvent::RIGHT_BUTTON_UP:
            break;
        case window::MouseEvent::CURSOR_MOVED:
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

    void handle_keyboard_event(window::KeyEvent &key, window::KeyState &key_state) {
        switch (key) {
        case window::KeyEvent::T: {
            if (key_state != window::KeyState::DOWN) {
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
        case window::KeyEvent::R: {
            if (!selected_entity_id.has_value()) {
                return;
            }

            if (key_state == window::KeyState::DOWN) {
                ecs.update_component<RigidBody>(
                    selected_entity_id.value(),
                    [](RigidBody &r) { r.angular_velocity = glm::radians(45.0); });

            } else if (key_state == window::KeyState::UP) {
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
        e1, std::move(RenderBodyBuilder().color(colors::RED).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2, RigidBodyBuilder()
                                         .position(WorldPoint(200.0f, 0.0, 0.0))
                                         .shape(Shape::create_triangle_data(200.0))
                                         .collision_restitution(0.3f)
                                         .build());
    ecs.add_component<RenderBody>(e2, RenderBodyBuilder().color(colors::GREEN).build());
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
        e1, std::move(RenderBodyBuilder().color(colors::RED).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 RigidBodyBuilder()
                                     .position(WorldPoint(200.0f, 0.0, 0.0))
                                     .shape(Shape::create_rectangle_data(180.0, 180.0))
                                     .collision_restitution(0.3f)
                                     .build());
    ecs.add_component<RenderBody>(e2, RenderBodyBuilder().color(colors::GREEN).build());
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
        e1, std::move(RenderBodyBuilder().color(colors::RED).build()));

    EntityId e2 = ecs.create_entity();
    ecs.add_component<RigidBody>(e2,
                                 RigidBodyBuilder()
                                     .position(WorldPoint(200.0f, 0.0, 0.0))
                                     .shape(Shape::create_rectangle_data(180.0, 180.0))
                                     .collision_restitution(0.3f)
                                     .build());
    ecs.add_component<RenderBody>(e2, RenderBodyBuilder().color(colors::GREEN).build());
}

int main() {
    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "0_collision_detection"}};

    auto game = std::make_unique<Example0CollisionDetection>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);
    std::cout << std::endl << std::endl;
    std::cout << "Control:" << std::endl;
    std::cout << "  Left Mouse - move selected shape around" << std::endl;
    std::cout << "  T          - to toggle between shapes" << std::endl;
    std::cout << "  R          - hold to rotate selected shape (while holding Left Mouse)"
              << std::endl;

    std::cout << std::endl << "=====================================" << std::endl;
    std::cout << "============== WARNING ==============" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "The click position calculations are wrong because I changed them to"
              << std::endl;
    std::cout << "align with the GPU coordinate system and I could not be bothered to"
              << std::endl;
    std::cout << "fix this example." << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << std::endl << std::endl;

    game_engine->run();

    return 0;
}
