#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "render_engine/RenderBody.h"
#include "render_engine/WindowConfig.h"
#include "render_engine/fonts/Font.h"
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
                                     .uvwt(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))
                                     .build());

        ecs.add_component<RenderBody>(ecs.create_entity(),
                                      RenderBodyBuilder()
                                          .position(WorldPoint(0, 300))
                                          .shape(Shape::create_hexagon_data(80.0f))
                                          .color(glm::vec3(1.0, 0.0, 0.0))
                                          .build());
        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-292, 192))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(glm::vec3(0.0, 0.0, 1.0))
                                     .uvwt(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f))
                                     .build());

        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-208, 192))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(glm::vec3(0.0, 0.0, 1.0))
                                     .uvwt(glm::vec4(0.5f, 0.0f, 1.0f, 0.5f))
                                     .build());

        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-292, 108))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(glm::vec3(0.0, 0.0, 1.0))
                                     .uvwt(glm::vec4(0.0f, 0.5f, 0.5f, 1.0f))
                                     .build());

        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-208, 108))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(glm::vec3(0.0, 0.0, 1.0))
                                     .uvwt(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f))
                                     .build());
    };

    ~ShapeRendering() {};

    void update(float dt) override {};

    void render(RenderEngine &render_engine) override {
        std::vector<std::reference_wrapper<const RenderBody>> render_bodies = {};
        for (auto it = ecs.begin<RenderBody>(); it != ecs.end<RenderBody>(); it++) {
            render_bodies.push_back(ecs.get_component<RenderBody>(it.id()).value());
        }

        bool success = render_engine.begin_render_pass();
        if (!success) {
            return;
        }

        render_engine.render(render_bodies);
        render_engine.render_text("ADAM", glm::vec2(0.0f, 0.0f), 128);
        render_engine.render_text("LINDA", glm::vec2(0.0f, 100.0f), 64);

        render_engine.render_ui();

        success = render_engine.end_render_pass();
        if (!success) {
            return;
        }
    };

    void setup(RenderEngine &render_engine) override {}
};

int main() {

    GameEngineConfig config{
        .window_config = WindowConfig{.dims = WindowDimensions(800, 800),
                                      .title = "2_shape_rendering"},
        .use_font = UseFont::Default,
    };

    auto game = std::make_unique<ShapeRendering>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
