#include "Coordinates.h"
#include "EntityComponentStorage.h"
#include "Game.h"
#include "GameEngine.h"
#include "Shape.h"
#include "physics_engine/SAT.h"
#include "render_engine/RenderBody.h"
#include "render_engine/Window.h"
#include <memory>
#include <optional>

// DONE: Implement SAT
// DONE: Either click inside does not fully work or the rendering does not correspond to
//       the rigidbody (click on triangle does not work properly)
// DONE: Implement EntityComponentStorage
// DONE: Implement SAT example
//        - Add rectangle rendering
// TODO: Implement CollisionResponse
// TODO: Include shader code into game_engine lib
class Dev : public Game {
  public:
    EntityComponentStorage ecs;
    std::optional<size_t> selected_entity_id;
    WorldPoint selected_entity_cursor_offset;
    WorldPoint camera_center;

    std::optional<CollisionInformation> collision_point;

    Dev()
        : selected_entity_id(std::nullopt),
          selected_entity_cursor_offset(WorldPoint(0.0, 0.0, 0.0)),
          camera_center(WorldPoint(400.0f, 400.0f, 0.0f)) {

        Entity e1 = {.rigid_body = {.position = WorldPoint(-200.0f, 0.0, 0.0),
                                    .rotation = glm::radians(0.0f),
                                    .shape = Shape::create_triangle_data(200.0)},
                     .render_body = {.color = glm::vec3(1.0, 1.0, 0.0)}};
        /*ecs.add_entity(e1);*/

        Entity e2 = {.rigid_body = {.position = WorldPoint(0.0f, 200.0, 0.0),
                                    .rotation = glm::radians(0.0f),
                                    .shape = Shape::create_triangle_data(200.0)},
                     .render_body = {.color = glm::vec3(0.0, 0.0, 1.0)}};
        ecs.add_entity(e2);

        Entity e3 = {.rigid_body = {.position = WorldPoint(200.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(200.0, 200.0)},
                     .render_body = {.color = glm::vec3(0.0, 0.5, 0.5)}};
        /*ecs.add_entity(e3);*/

        Entity e4 = {.rigid_body = {.position = WorldPoint(200.0, -200.0, 0.0),
                                    .rotation = 0.0,
                                    .shape = Shape::create_rectangle_data(400.0, 200.0)},
                     .render_body = {.color = glm::vec3(0.0, 0.0, 5.0)}};
        ecs.add_entity(e4);

        collision_point = std::nullopt;
    };

    ~Dev() {};

    void update() override {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        /*ecs.rigid_bodies[0].rotation = (time / 5.0) * glm::radians(90.0);*/
        /*ecs.rigid_bodies[1].rotation = (time / 5.0) * glm::radians(-90.0);*/

        RigidBody body_a = ecs.rigid_bodies[0];
        RigidBody body_b = ecs.rigid_bodies[1];
        std::optional<CollisionInformation> collisions =
            SAT::collision_detection(body_a, body_b);

        collision_point = collisions;
    };

    void render(RenderEngine &render_engine) override {
        std::vector<RenderBody> render_bodies = {};
        for (int i = 0; i < ecs.rigid_bodies.size(); i++) {
            render_bodies.push_back(ecs.get_render_body(i));
        }

        if (collision_point.has_value()) {
            render_bodies.push_back(
                RenderBody{.position = glm::vec3(collision_point->collision_point.x,
                                                 collision_point->collision_point.y, 0.0),
                           .color = glm::vec3(0.0, 1.0, 0.0),
                           .rotation = 0.0,
                           .shape = Shape::create_rectangle_data(10.0, 10.0)});
        }
        render_engine.render(render_bodies);
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this](MouseEvent e, ViewportPoint &p) { this->handle_mouse_event(e, p); });
    }

    void handle_mouse_event(MouseEvent event, ViewportPoint &point) {
        switch (event) {
        case MouseEvent::LEFT_BUTTON_UP:
            selected_entity_id = std::nullopt;
            selected_entity_cursor_offset.x = 0.0;
            selected_entity_cursor_offset.y = 0.0;
            selected_entity_cursor_offset.z = 0.0;
            break;
        case MouseEvent::LEFT_BUTTON_DOWN: {
            WorldPoint wp = point.to_world_point(camera_center);
            std::vector<glm::vec3> vertices = ecs.rigid_bodies[0].vertices();

            for (size_t i = 0; i < ecs.rigid_bodies.size(); i++) {
                RigidBody body = ecs.rigid_bodies[i];
                if (body.is_point_inside(wp)) {
                    selected_entity_id = i;
                    selected_entity_cursor_offset.x =
                        ecs.rigid_bodies[i].position.x - wp.x;
                    selected_entity_cursor_offset.y =
                        ecs.rigid_bodies[i].position.y - wp.y;
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
                ecs.set_entity_position(entity_handle,
                                        wp.x + selected_entity_cursor_offset.x,
                                        wp.y + selected_entity_cursor_offset.y);
            }
            break;
        default:
            break;
        }
    }
};

int main() {
    GameEngineConfig config{};
    config.window_width = 800;
    config.window_height = 800;
    config.window_title = "Game Window";

    auto game = std::make_unique<Dev>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);
    game_engine->run();

    return 0;
}
