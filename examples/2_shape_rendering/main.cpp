#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "render_engine/RenderBody.h"
#include "shape.h"
#include <functional>
#include <memory>
#include <optional>

class ShapeRendering : public Game {
  public:
    EntityComponentStorage ecs;

    ShapeRendering() : ecs(EntityComponentStorage()) {
        ecs.add_component<RenderBody>(ecs.create_entity(),
                                      RenderBodyBuilder()
                                          .position(WorldPoint(-300, 300))
                                          .shape(Shape::create_circle_data(80.0f))
                                          .color(glm::vec3(1.0, 0.0, 0.0))
                                          .build());

        ecs.add_component<RenderBody>(ecs.create_entity(),
                                      RenderBodyBuilder()
                                          .position(WorldPoint(-200, 300))
                                          .shape(Shape::create_triangle_data(80.0f))
                                          .color(glm::vec3(0.0, 1.0, 0.0))
                                          .build());

        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-100, 300))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(glm::vec3(0.0, 0.0, 1.0))
                                     .build());

        ecs.add_component<RenderBody>(ecs.create_entity(),
                                      RenderBodyBuilder()
                                          .position(WorldPoint(0, 300))
                                          .shape(Shape::create_hexagon_data(80.0f))
                                          .color(glm::vec3(1.0, 0.0, 0.0))
                                          .build());
    };

    ~ShapeRendering() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        std::vector<std::reference_wrapper<const RenderBody>> render_bodies = {};
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
    config.window_title = "2_shape_rendering";

    auto game = std::make_unique<ShapeRendering>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
