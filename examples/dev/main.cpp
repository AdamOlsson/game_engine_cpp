#include "EntityBuilder.h"
#include "EntityComponentStorage.h"
#include "Game.h"
#include "GameEngine.h"
#include "render_engine/RenderBody.h"
#include <memory>

// TODO: Implement click and move triangles
// TODO: Implement SAT
// TODO: Implement SAT example
// TODO: Implement EntityComponentStorage

class Dev : public Game {
  public:
    EntityComponentStorage ecs;
    std::shared_ptr<uint32_t> click_count;

    Dev() : click_count(std::make_shared<uint32_t>(0)) {

        EntityBuilder e1{};
        e1.position = glm::vec3(-0.5f, 0.0f, 0.0f);
        e1.rotation = 0.0;
        e1.color = glm::vec3(1.0, 0.0, 0.0);
        ecs.add_entity(e1);

        EntityBuilder e2{};
        e2.position = glm::vec3(0.5f, 0.0f, 0.0f);
        e2.rotation = 0.0;
        e2.color = glm::vec3(0.0, 1.0, 0.0);
        ecs.add_entity(e2);
    };

    ~Dev() {};

    void update() override {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        ecs.update_rotation(0, (time / 5.0) * glm::radians(90.0));
        ecs.update_rotation(1, (time / 5.0) * glm::radians(-90.0));
    };

    void render(RenderEngine &render_engine) override {
        std::vector<RenderBody> render_bodies = {ecs.get_render_body(0),
                                                 ecs.get_render_body(1)};
        render_engine.render(render_bodies);
    };

    void setup(RenderEngine &render_engine) override {
        render_engine.register_mouse_event_callback(
            [this]() { this->increase_click_count(); });
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
