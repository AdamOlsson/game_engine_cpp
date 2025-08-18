#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "render_engine/RenderBody.h"
#include "render_engine/RenderEngine.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/window/WindowConfig.h"
#include "shape.h"
#include <functional>
#include <memory>
#include <optional>

class ShapeRendering : public Game {
  private:
    std::unique_ptr<RenderEngine> m_render_engine; // TODO: Remove
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;
    /*std::unique_ptr<ui::UIPipeline> m_ui_pipeline;*/

  public:
    EntityComponentStorage ecs;

    ShapeRendering() : ecs(EntityComponentStorage()) {
        ecs.add_component<RenderBody>(ecs.create_entity(),
                                      RenderBodyBuilder()
                                          .position(WorldPoint(-300, 300))
                                          .shape(Shape::create_circle_data(80.0f))
                                          .color(colors::RED)
                                          .build());

        ecs.add_component<RenderBody>(ecs.create_entity(),
                                      RenderBodyBuilder()
                                          .position(WorldPoint(-200, 300))
                                          .shape(Shape::create_triangle_data(80.0f))
                                          .color(colors::GREEN)
                                          .build());

        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-100, 300))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(colors::BLUE)
                                     .uvwt(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))
                                     .build());

        ecs.add_component<RenderBody>(ecs.create_entity(),
                                      RenderBodyBuilder()
                                          .position(WorldPoint(0, 300))
                                          .shape(Shape::create_hexagon_data(80.0f))
                                          .color(colors::RED)
                                          .build());
        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-292, 192))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(colors::BLUE)
                                     .uvwt(glm::vec4(0.0f, 0.0f, 0.5f, 0.5f))
                                     .build());

        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-208, 192))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(colors::BLUE)
                                     .uvwt(glm::vec4(0.5f, 0.0f, 1.0f, 0.5f))
                                     .build());

        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-292, 108))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(colors::BLUE)
                                     .uvwt(glm::vec4(0.0f, 0.5f, 0.5f, 1.0f))
                                     .build());

        ecs.add_component<RenderBody>(
            ecs.create_entity(), RenderBodyBuilder()
                                     .position(WorldPoint(-208, 108))
                                     .shape(Shape::create_rectangle_data(80.0f, 80.0f))
                                     .color(colors::WHITE)
                                     .uvwt(glm::vec4(0.5f, 0.5f, 1.0f, 1.0f))
                                     .build());
    };

    ~ShapeRendering() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {
        register_all_shaders();
        register_all_fonts();
        register_all_images();
        m_swap_chain_manager = std::make_unique<SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<CommandBufferManager>(
            ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT);

        m_render_engine = std::make_unique<RenderEngine>(
            ctx, m_command_buffer_manager.get(), m_swap_chain_manager.get(),
            UseFont::Default); // TODO: remove
    }

    void render() override {
        std::vector<std::reference_wrapper<const RenderBody>> render_bodies = {};
        for (auto it = ecs.begin<RenderBody>(); it != ecs.end<RenderBody>(); it++) {
            render_bodies.push_back(ecs.get_component<RenderBody>(it.id()).value());
        }

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        RenderPass render_pass = m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        m_render_engine->render(command_buffer, render_bodies);

        m_render_engine->render_text(
            command_buffer, ui::TextBox("ADAM", ui::ElementProperties{.font.size = 128}));
        m_render_engine->render_text(
            command_buffer,
            ui::TextBox("LINDA",
                        ui::ElementProperties{.container.center = glm::vec2(0.0f, 100.0f),
                                              .font.size = 64}));

        render_pass.end_submit_present();
    };
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "2_shape_rendering"},
        .use_font = UseFont::Default,
    };

    auto game = std::make_unique<ShapeRendering>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
