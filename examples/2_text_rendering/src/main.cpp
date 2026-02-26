#include "camera/Camera.h"
#include "font/FontLoader.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/SwapChainManager.h"

#define ASSET_FILE(filename) ASSET_DIR "/" filename
constexpr auto INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr auto ZOOM_SCALE_FACTOR = 0.05f;

class ExampleTextRendering : public Game {
  private:
    std::unique_ptr<vulkan::SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    bool m_is_left_mouse_pressed = false;
    window::ViewportPoint m_mouse_last_position = window::ViewportPoint();
    camera::Camera2D m_camera;

    std::unique_ptr<graphics_pipeline::text::TextRenderer> m_text_renderer;

    std::vector<graphics_pipeline::text::TextString> m_texts;

  public:
    ExampleTextRendering()
        : m_mouse_last_position(window::ViewportPoint(-10000, -1000)) {}

    ~ExampleTextRendering() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {

        font::FontLoader font_loader = font::FontLoader(
            /*ASSET_FILE("Quaaykop-DYE1R.ttf")*/
            ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf")
            /*ASSET_FILE("ftystrategycidencv.otf")*/
            /*ASSET_FILE("TypeLightSans-KV84p.otf")*/
        );

        auto window_size = ctx->window->get_framebuffer_size<float>();
        const float num_pixels_at_default_zoom = 200.0f;
        m_camera = camera::Camera2D(window_size.width, window_size.height,
                                    num_pixels_at_default_zoom);
        m_camera.configure_max_zoom(5.0f);
        m_camera.configure_min_zoom(0.001f);
        m_camera.set_zoom(1.0f);
        register_mouse_event_handler(ctx.get());

        m_swap_chain_manager = std::make_unique<vulkan::SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        auto push_constant_range =
            vulkan::PushConstantRange{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                      .offset = 0,
                                      .size = camera::Camera2D::matrix_size()};

        m_text_renderer = std::make_unique<graphics_pipeline::text::TextRenderer>(
            ctx, m_swap_chain_manager.get(), &push_constant_range);

        m_text_renderer->load_font(m_command_buffer_manager.get(),
                                   std::move(font_loader));

        std::string sentence = "Pack my box with five dozen liquor jugs.";
        m_texts.push_back(
            m_text_renderer->create_text(sentence, graphics_pipeline::text::TextOpts{
                                                       .position = math::Vector2(0, 0),
                                                       .font_color = util::colors::WHITE,
                                                       .font_size = 11,
                                                   }));

        m_texts.push_back(
            m_text_renderer->create_text(sentence, graphics_pipeline::text::TextOpts{
                                                       .position = math::Vector2(0, 22),
                                                       .font_color = util::colors::RED,
                                                       .font_size = 17,
                                                   }));

        m_texts.push_back(
            m_text_renderer->create_text(sentence, graphics_pipeline::text::TextOpts{
                                                       .position = math::Vector2(0, 50),
                                                       .font_color = util::colors::BLUE,
                                                       .font_size = 24,
                                                   }));
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

        glm::mat4 push_constant = m_camera.get_view_projection_matrix();

        for (const auto &text : m_texts) {
            m_text_renderer->render(command_buffer, text, &push_constant);
        }

        render_pass.end_submit_present();
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
