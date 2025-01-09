#include "EntityComponentStorage.h"
#include "Game.h"
#include "GameEngine.h"
#include "render_engine/RenderBody.h"
#include "shape.h"
#include <memory>

// TODO: Implement SAT
// TODO: Implement SAT example
// TODO: Implement EntityComponentStorage

// TODO: Include shader code into game_engine lib
class Dev : public Game {
  public:
    EntityComponentStorage ecs;
    std::shared_ptr<uint32_t> click_count;

    Dev() : click_count(std::make_shared<uint32_t>(0)) {

        Entity e1 = {.rigid_body = {.position = glm::vec3(200.0, 400.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = create_triangle_data(400.0)},
                     .render_body = {.color = glm::vec3(1.0, 1.0, 0.0)}};
        ecs.add_entity(e1);

        Entity e2 = {.rigid_body = {.position = glm::vec3(600.0, 400.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = create_triangle_data(250.0)},
                     .render_body = {.color = glm::vec3(1.0, 0.0, 0.0)}};
        ecs.add_entity(e2);
    };

    ~Dev() {};

    void update() override {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        ecs.rigid_bodies[0].rotation = (time / 5.0) * glm::radians(90.0);
        ecs.rigid_bodies[1].rotation = (time / 5.0) * glm::radians(-90.0);
    };

    void render(RenderEngine &render_engine) override {
        std::vector<RenderBody> render_bodies = {ecs.get_render_body(0),
                                                 ecs.get_render_body(1)};
        render_engine.render(render_bodies);
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this](double xpos, double ypos) { this->select_entity(xpos, ypos); });
    }

    void select_entity(double xpos, double ypos) {
        glm::vec2 click_point = glm::vec2(xpos, ypos);
        for (RigidBody body : ecs.rigid_bodies) {
            body.is_point_inside(click_point);
        }
    }

    // Temporary to test out event callbacks
    void increase_click_count() {
        (*this->click_count)++;
        std::cout << "Click count: " << *this->click_count << std::endl;
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
