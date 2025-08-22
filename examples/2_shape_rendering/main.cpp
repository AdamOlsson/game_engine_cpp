#include "Coordinates.h"
#include "Game.h"
#include "GameEngine.h"
#include "entity_component_storage/ComponentStore.h"
#include "entity_component_storage/EntityComponentStorage.h"
#include "render_engine/RenderBody.h"
#include "render_engine/colors.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "render_engine/graphics_pipeline/TextPipeline.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/TextBox.h"
#include "render_engine/window/WindowConfig.h"
#include "shape.h"
#include <functional>
#include <memory>
#include <optional>

class ShapeRendering : public Game {
  private:
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;

    std::unique_ptr<Texture> m_dog_image;
    vulkan::Sampler m_sampler;
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;
    std::unique_ptr<graphics_pipeline::TextPipeline> m_text_pipeline;

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
                                          .color(colors::RED)
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

        m_sampler = vulkan::Sampler(ctx);
        m_dog_image = Texture::unique_from_image_resource_name(
            ctx, m_command_buffer_manager.get(), "DogImage");
        m_geometry_pipeline = std::make_unique<graphics_pipeline::GeometryPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager,
            graphics_pipeline::GeometryPipelineOptions{
                .combined_image_sampler =
                    vulkan::DescriptorImageInfo(m_dog_image->view(), &m_sampler)});

        auto font = std::make_unique<Font>(ctx, m_command_buffer_manager.get(),
                                           "DefaultFont", &m_sampler);
        m_text_pipeline = std::make_unique<graphics_pipeline::TextPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager, std::move(font));
    }

    void render() override {
        std::vector<std::reference_wrapper<const RenderBody>> render_bodies = {};
        for (auto it = ecs.begin<RenderBody>(); it != ecs.end<RenderBody>(); it++) {
            render_bodies.push_back(ecs.get_component<RenderBody>(it.id()).value());
        }

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        RenderPass render_pass = m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        // START RENDERING GEOMETRIES
        auto &circle_instance_buffer = m_geometry_pipeline->get_circle_instance_buffer();
        auto &triangle_instance_buffer =
            m_geometry_pipeline->get_triangle_instance_buffer();
        auto &rectangle_instance_buffer =
            m_geometry_pipeline->get_rectangle_instance_buffer();
        auto &hexagon_instance_buffer =
            m_geometry_pipeline->get_hexagon_instance_buffer();

        circle_instance_buffer.clear();
        triangle_instance_buffer.clear();
        rectangle_instance_buffer.clear();
        hexagon_instance_buffer.clear();

        rectangle_instance_buffer.emplace_back(
            glm::vec3(150.0f, 200.0f, 0.0), colors::TRANSPARENT, 0.0f,
            glm::vec2(400.0f, 100.0f), glm::vec4(-1.0f),
            graphics_pipeline::GeometryInstanceBufferObject::BorderProperties{
                .color = colors::WHITE, .thickness = 10.0f, .radius = 15.0f});

        for (auto &b : render_bodies) {
            auto deref_b = b.get();
            switch (deref_b.shape.encode_shape_type()) {
            case ShapeTypeEncoding::CircleShape:
                circle_instance_buffer.emplace_back(
                    deref_b.position, deref_b.color, deref_b.rotation,
                    deref_b.shape.get<Circle>(), deref_b.uvwt,
                    graphics_pipeline::GeometryInstanceBufferObject::BorderProperties{
                        .color = colors::YELLOW, .thickness = 10.0f});
                break;
            case ShapeTypeEncoding::TriangleShape:
                triangle_instance_buffer.emplace_back(
                    deref_b.position, deref_b.color, deref_b.rotation, deref_b.shape,
                    deref_b.uvwt,
                    graphics_pipeline::GeometryInstanceBufferObject::BorderProperties{
                        .color = colors::YELLOW, .thickness = 5.0f});
                break;
            case ShapeTypeEncoding::RectangleShape:
                rectangle_instance_buffer.emplace_back(
                    deref_b.position, deref_b.color, deref_b.rotation,
                    deref_b.shape.get<Rectangle>(), deref_b.uvwt,
                    graphics_pipeline::GeometryInstanceBufferObject::BorderProperties{
                        colors::YELLOW, 5.0f, 5.0f});
                break;
            case ShapeTypeEncoding::HexagonShape:
                hexagon_instance_buffer.emplace_back(
                    deref_b.position, deref_b.color, deref_b.rotation, deref_b.shape,
                    deref_b.uvwt,
                    graphics_pipeline::GeometryInstanceBufferObject::BorderProperties{
                        colors::YELLOW, 5.0f});
                break;
            };
        }

        circle_instance_buffer.transfer();
        triangle_instance_buffer.transfer();
        rectangle_instance_buffer.transfer();
        hexagon_instance_buffer.transfer();

        m_geometry_pipeline->render_circles(command_buffer);
        m_geometry_pipeline->render_triangles(command_buffer);
        m_geometry_pipeline->render_rectangles(command_buffer);
        m_geometry_pipeline->render_hexagons(command_buffer);
        // STOP RENDERING GEOMETRIES

        const auto text_box_1 =
            ui::TextBox("ADAM", ui::ElementProperties{.font.size = 128});

        const auto text_box_2 =
            ui::TextBox("LINDA", ui::ElementProperties{.container.center =
                                                           glm::vec3(0.0f, 100.0f, 0.0f),
                                                       .font.size = 64});
        auto &character_instance_buffer = m_text_pipeline->get_character_buffer();
        auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();

        character_instance_buffer.clear();
        text_segment_buffer.clear();

        // TODO: This invokation also writes to the instance buffer, make the calling
        // conventions equal to that of the GeometryPipeline
        m_text_pipeline->text_kerning(text_box_1.text, text_box_1.properties);
        m_text_pipeline->text_kerning(text_box_2.text, text_box_2.properties);

        character_instance_buffer.transfer();
        text_segment_buffer.transfer();

        m_text_pipeline->render_text(command_buffer);

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
