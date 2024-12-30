#include "Entity.h"
#include "Game.h"
#include "GameEngine.h"
#include "physics_engine/RigidBody.h"
#include "render_engine/RenderBody.h"
#include <memory>

// TODO: Implement click and move triangles
// TODO: Implement SAT
// TODO: Implement SAT example
// TODO: Implement EntityComponentStorage

class Dev : public Game {
  public:
    std::vector<std::unique_ptr<Entity>> entities;

    std::shared_ptr<uint32_t> click_count;

    Dev() : click_count(std::make_shared<uint32_t>(0)) {

        entities.push_back(std::make_unique<Entity>(
            std::make_unique<RigidBody>(glm::vec3(0.5f, 0.0f, 0.0f), 0.0f),
            std::make_unique<RenderBody>()));
        entities.push_back(std::make_unique<Entity>(
            std::make_unique<RigidBody>(glm::vec3(-0.5f, 0.0f, 0.0f), 0.0f),
            std::make_unique<RenderBody>()));
    };

    ~Dev() {};

    void update() override {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        entities[0]->rigid_body->rotation = (time / 5.0) * glm::radians(90.0);
        entities[1]->rigid_body->rotation = (time / 5.0) * glm::radians(-90.0);
    };

    void render(RenderEngine &render_engine) override {
        entities[0]->render_body->position = entities[0]->rigid_body->position;
        entities[1]->render_body->position = entities[1]->rigid_body->position;

        entities[0]->render_body->rotation = entities[0]->rigid_body->rotation;
        entities[1]->render_body->rotation = entities[1]->rigid_body->rotation;

        std::vector<RenderBody *> render_bodies = {entities[0]->render_body.get(),
                                                   entities[1]->render_body.get()};
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
