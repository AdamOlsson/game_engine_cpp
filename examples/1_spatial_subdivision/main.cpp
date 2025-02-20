#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "render_engine/RenderBody.h"
#include <functional>
#include <memory>
#include <optional>

class Example1SpatialSubdivision : public Game {
  public:
    EntityComponentStorage ecs;

    Example1SpatialSubdivision() : ecs(EntityComponentStorage()) {};

    ~Example1SpatialSubdivision() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        std::vector<std::reference_wrapper<const RenderBody>> render_bodies = {};

        // TODO: The common values are only copied during the get_component() function,
        // therefore we can't use direct access with RenderBodies
        for (auto it = ecs.begin<RenderBody>(); it != ecs.end<RenderBody>(); it++) {
            render_bodies.push_back(ecs.get_component<RenderBody>(it.id()).value());
        }

        render_engine.render(render_bodies);
    };

    void setup(RenderEngine &render_engine) override {}
};

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
