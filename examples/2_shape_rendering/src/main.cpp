#include "game_engine_sdk/Dimension.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/WorldPoint.h"
#include "game_engine_sdk/render_engine/Camera.h"
#include "game_engine_sdk/render_engine/colors.h"
#include "game_engine_sdk/render_engine/fonts/Font.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/TextPipeline.h"
#include "game_engine_sdk/render_engine/resources/ResourceManager.h"
#include "game_engine_sdk/render_engine/ui/ElementProperties.h"
#include "game_engine_sdk/render_engine/ui/TextBox.h"
#include "game_engine_sdk/render_engine/window/WindowConfig.h"
#include "game_engine_sdk/shape.h"
#include <memory>

class ShapeRendering : public Game {
  private:
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;

    std::unique_ptr<Texture> m_dog_image;
    vulkan::Sampler m_sampler;
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;
    std::unique_ptr<graphics_pipeline::TextPipeline> m_text_pipeline;

    std::vector<graphics_pipeline::GeometryInstanceBufferObject> m_geometries;
    std::vector<shape::Shape> m_shape_ids;

  public:
    ShapeRendering() {
        m_shape_ids.push_back(shape::Shape::Circle);
        m_geometries.push_back(graphics_pipeline::GeometryInstanceBufferObject{
            .center = WorldPoint(-300, 300),
            .dimension = Dimension(80.0f),
            .color = colors::RED,
            .border =
                {
                    .color = colors::YELLOW,
                    .thickness = 10.0f,
                },
        });

        m_shape_ids.push_back(shape::Shape::Triangle);
        m_geometries.push_back(graphics_pipeline::GeometryInstanceBufferObject{
            .center = WorldPoint(-200, 300),
            .dimension = Dimension(80.0f),
            .color = colors::RED,
            .border = {
                .color = colors::YELLOW,
                .thickness = 5.0f,
            }});

        m_shape_ids.push_back(shape::Shape::Rectangle);
        m_geometries.push_back(graphics_pipeline::GeometryInstanceBufferObject{
            .center = WorldPoint(-100, 300),
            .dimension = Dimension(80.0f, 80.0f),
            .color = colors::BLUE,
            .uvwt = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
            .border = {
                .color = colors::YELLOW,
                .thickness = 5.0f,
                .radius = 5.0f,
            }});

        m_shape_ids.push_back(shape::Shape::Hexagon);
        m_geometries.push_back(
            graphics_pipeline::GeometryInstanceBufferObject{.center = WorldPoint(0, 300),
                                                            .dimension = Dimension(80.0f),
                                                            .color = colors::RED,
                                                            .border = {
                                                                .color = colors::YELLOW,
                                                                .thickness = 5.0f,
                                                            }});

        m_shape_ids.push_back(shape::Shape::Rectangle);
        m_geometries.push_back(graphics_pipeline::GeometryInstanceBufferObject{
            .center = WorldPoint(-292, 192),
            .dimension = Dimension(80.0f, 80.0f),
            .color = colors::BLUE,
            .uvwt = glm::vec4(0.0f, 0.0f, 0.5f, 0.5f),
            .border = {
                .color = colors::YELLOW,
                .thickness = 5.0f,
                .radius = 5.0f,
            }});

        m_shape_ids.push_back(shape::Shape::Rectangle);
        m_geometries.push_back(graphics_pipeline::GeometryInstanceBufferObject{
            .center = WorldPoint(-208, 192),
            .dimension = Dimension(80.0f, 80.0f),
            .color = colors::BLUE,
            .uvwt = glm::vec4(0.5f, 0.0f, 1.0f, 0.5f),
            .border = {
                .color = colors::YELLOW,
                .thickness = 5.0f,
                .radius = 5.0f,
            }});

        m_shape_ids.push_back(shape::Shape::Rectangle);
        m_geometries.push_back(graphics_pipeline::GeometryInstanceBufferObject{
            .center = WorldPoint(-292, 108),
            .dimension = Dimension(80.0f, 80.0f),
            .color = colors::BLUE,
            .uvwt = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f),
            .border =
                {
                    .color = colors::YELLOW,
                    .thickness = 5.0f,
                    .radius = 5.0f,
                }

        });

        m_shape_ids.push_back(shape::Shape::Rectangle);
        m_geometries.push_back(graphics_pipeline::GeometryInstanceBufferObject{
            .center = WorldPoint(-208, 108),
            .dimension = Dimension(80.0f, 80.0f),
            .color = colors::WHITE,
            .uvwt = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f),
            .border = {
                .color = colors::YELLOW,
                .thickness = 5.0f,
                .radius = 5.0f,
            }});

        m_shape_ids.push_back(shape::Shape::Rectangle);
        m_geometries.push_back(graphics_pipeline::GeometryInstanceBufferObject{
            .center = WorldPoint(150.0f, 200.0f, 0.0),
            .dimension = Dimension(400.0f, 100.0f),
            .color = colors::TRANSPARENT,
            .uvwt = glm::vec4(-1.0f),
            .border = {.color = colors::WHITE, .thickness = 10.0f, .radius = 15.0f}

        });
    };

    ~ShapeRendering() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {
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
                .combined_image_samplers = {
                    vulkan::DescriptorImageInfo(m_dog_image->view(), &m_sampler)}});

        auto font = std::make_unique<Font>(ctx, m_command_buffer_manager.get(),
                                           "DefaultFont", &m_sampler);
        m_text_pipeline = std::make_unique<graphics_pipeline::TextPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager, std::move(font));
    }

    void render() override {

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

        const size_t num_geometries = m_geometries.size();
        for (auto i = 0; i < num_geometries; i++) {
            switch (m_shape_ids[i]) {
            case shape::Shape::None:
                break;
            case shape::Shape::Circle:
                circle_instance_buffer.push_back(m_geometries[i]);
                break;
            case shape::Shape::Triangle:
                triangle_instance_buffer.push_back(m_geometries[i]);
                break;
            case shape::Shape::Rectangle:
                rectangle_instance_buffer.push_back(m_geometries[i]);
                break;
            case shape::Shape::Hexagon:
                hexagon_instance_buffer.push_back(m_geometries[i]);
                break;
            }
        }

        circle_instance_buffer.transfer();
        triangle_instance_buffer.transfer();
        rectangle_instance_buffer.transfer();
        hexagon_instance_buffer.transfer();

        auto camera_transform_matrix = Camera2D::get_default_view_matrix();
        m_geometry_pipeline->render_circles(command_buffer, camera_transform_matrix);
        m_geometry_pipeline->render_triangles(command_buffer, camera_transform_matrix);
        m_geometry_pipeline->render_rectangles(command_buffer, camera_transform_matrix);
        m_geometry_pipeline->render_hexagons(command_buffer, camera_transform_matrix);
        // STOP RENDERING GEOMETRIES

        const auto text_box_1 =
            ui::TextBox("ADAM", ui::ElementProperties{.font = {.size = 128}});

        const auto text_box_2 = ui::TextBox(
            "LINDA",
            ui::ElementProperties{.container = {.center = glm::vec3(0.0f, 100.0f, 0.0f)},
                                  .font = {.size = 64}});
        auto &character_instance_buffer = m_text_pipeline->get_character_buffer();
        auto &text_segment_buffer = m_text_pipeline->get_text_segment_buffer();

        character_instance_buffer.clear();
        text_segment_buffer.clear();

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
    };

    auto game = std::make_unique<ShapeRendering>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
