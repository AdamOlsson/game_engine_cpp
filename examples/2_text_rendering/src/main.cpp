#include "camera/Camera.h"
#include "font/GlyphNames.h"
#include "font/OTFFont.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/render_engine/ModelMatrix.h"
#include "graphics_pipeline/text/TextPipeline.h"
#include "graphics_pipeline/text/TextPipelineDescriptorSet.h"
#include "graphics_pipeline/text/TextPipelineSBO.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/SwapChainManager.h"

#define ASSET_FILE(filename) ASSET_DIR "/" filename
constexpr auto INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr auto ZOOM_SCALE_FACTOR = 0.1f;

// TODO:
// TextPipeline:
// - DONE Implement triangulation on on-curve points to get the interior of the glyph.
// - DONE Render the glyphs using the indices returned from the triangulation
// - DONE Implement Loop-Blinn Algorithm with the off-point curves to render smooth glyphs
// - DONE Handle glyphs with single holes such as A and D
// - DONE Handle glyphs with multiple holes such as B
// - DONE Handle compound glyphs with no holes such as i and !
// - DONE Handle compound glyphs with holes such as %
// - DONE Render the english alphabet for my font
// - DONE Implement all other Type2Charstring operators
// - Render the english alphabet for the two other fonts
//      - Make sure the Rabbid Highway font can be renderer
//      - Handle the glyph indexing not working in TypeLightSans font
// - Render sentences
//      - Parse kerning map
// - Refactor the usage and internals of OTFFont to be nice
// - Move ModelMatrix class to sdk

class ExampleTextRendering : public Game {
  private:
    std::unique_ptr<vulkan::SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    vulkan::DescriptorPool m_descriptor_pool;

    bool m_is_left_mouse_pressed = false;
    window::ViewportPoint m_mouse_last_position = window::ViewportPoint();
    camera::Camera2D m_camera;

    size_t m_num_instances;
    std::unique_ptr<
        vulkan::buffers::SwapStorageBuffer<graphics_pipeline::text::TextPipelineSBO>>
        m_glyph_positions;
    std::unique_ptr<graphics_pipeline::text::TextPipelineDescriptorSet> m_descriptor_set;
    std::unique_ptr<graphics_pipeline::text::TextPipeline> m_pipeline;

    font::OTFFont m_otf_font;

  public:
    ExampleTextRendering()
        : m_mouse_last_position(window::ViewportPoint(-10000, -1000)) {}

    ~ExampleTextRendering() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {

        font::OTFFont otf_font = font::OTFFont(ASSET_FILE("TypeLightSans-KV84p.otf"));
        /*font::OTFFont otf_font =*/
        /*    font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));*/
        m_otf_font = otf_font;

        m_glyph_positions = std::make_unique<
            vulkan::buffers::SwapStorageBuffer<graphics_pipeline::text::TextPipelineSBO>>(
            ctx, 2, otf_font.glyphs.size());

        const float scale = 0.1f;
        const size_t num_columns = 8;
        const float column_width = 100;
        const float row_height = 100;
        for (size_t i = 0; i < otf_font.glyphs.size(); i++) {
            const float x = static_cast<float>(i % num_columns) * column_width;
            const float y = static_cast<float>(i / num_columns) * row_height;

            // TODO: Each instance should represent one text (not only one glyph)
            m_glyph_positions->push_back(graphics_pipeline::text::TextPipelineSBO{
                .model_matrix = ModelMatrix().translate(x, y, 0.0f).scale(scale)});
            /*std::cout << otf_font.glyphs[i].name << " " << std::endl;*/
        }
        m_glyph_positions->transfer();

        auto window_size = ctx->window->get_framebuffer_size<float>();
        const float num_pixels_at_default_zoom = 200.0f;
        m_camera = camera::Camera2D(window_size.width, window_size.height,
                                    num_pixels_at_default_zoom);
        m_camera.configure_max_zoom(5.0f);
        m_camera.configure_min_zoom(0.2f);
        m_camera.set_zoom(0.4f);
        register_mouse_event_handler(ctx.get());

        m_swap_chain_manager = std::make_unique<vulkan::SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        m_descriptor_pool = vulkan::DescriptorPool(
            ctx, vulkan::DescriptorPoolOpts{.max_num_descriptor_sets = 2,
                                            .num_storage_buffers = 1,
                                            .num_uniform_buffers = 0,
                                            .num_combined_image_samplers = 0});

        const auto window_dims = ctx->window->dimensions<size_t>();

        m_descriptor_set =
            std::make_unique<graphics_pipeline::text::TextPipelineDescriptorSet>(
                ctx, m_descriptor_pool,
                graphics_pipeline::text::TextPipelineDescriptorSetOpts{
                    .storage_buffer_refs = vulkan::DescriptorBufferInfo::from_vector(
                        m_glyph_positions->get_buffer_references())});

        auto &descriptor_layout = m_descriptor_set->get_layout();
        auto push_constant_range =
            vulkan::PushConstantRange{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                      .offset = 0,
                                      .size = camera::Camera2D::matrix_size()};

        m_pipeline = std::make_unique<graphics_pipeline::text::TextPipeline>(
            ctx, m_command_buffer_manager.get(), m_swap_chain_manager.get(),
            &descriptor_layout, &push_constant_range);
        m_pipeline->load_font(m_command_buffer_manager.get(), otf_font);
    }

    void register_mouse_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_mouse_event_callback(
            [this](window::MouseEvent mouse_event, window::ViewportPoint &point) -> void {
                switch (mouse_event) {
                case window::MouseEvent::RIGHT_BUTTON_DOWN:
                    break;
                case window::MouseEvent::RIGHT_BUTTON_UP:
                    break;
                case window::MouseEvent::CURSOR_MOVED:
                    if (m_is_left_mouse_pressed) {
                        auto world_delta = m_camera.viewport_delta_to_world(
                            point - m_mouse_last_position);
                        m_camera.set_relative_position(world_delta * INVERT_AXISES);
                    }
                    m_mouse_last_position = point;
                    break;
                case window::MouseEvent::SCROLL:
                    m_camera.set_relative_zoom(point.y * ZOOM_SCALE_FACTOR);
                    break;
                case window::MouseEvent::LEFT_BUTTON_DOWN:
                    m_is_left_mouse_pressed = true;
                    break;
                case window::MouseEvent::LEFT_BUTTON_UP: {
                    m_is_left_mouse_pressed = false;
                    break;
                }
                }
            });
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        vulkan::RenderPass render_pass =
            m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        auto descriptor = m_descriptor_set.get();
        glm::mat4 push_constant = m_camera.get_view_projection_matrix();

        for (const font::Glyph &glyph : m_otf_font.glyphs) {
            char c = ' ';
            if (glyph.name.size() == 1) {
                c = glyph.name[0];
            } else if (glyph.name.size() > 1) {
                const std::optional<char> maybe_c = font::GlyphNames::to_char(glyph.name);
                if (maybe_c.has_value()) {
                    c = maybe_c.value();
                }
            }

            m_pipeline->render(command_buffer, descriptor, &push_constant,
                               font::Unicode(c));
        }

        render_pass.end_submit_present();

        /*m_gl->rotate();*/
        /*m_descriptor_set->rotate();*/
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "2_text_rendering"},
    };

    auto game = std::make_unique<ExampleTextRendering>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
