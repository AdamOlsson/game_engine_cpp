#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"
#include "window/WindowConfig.h"
#include <memory>

constexpr auto INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr auto ZOOM_SCALE_FACTOR = 0.1f;

class ExamplePathing : public Game {
  private:
    std::unique_ptr<vulkan::SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    vulkan::DescriptorPool m_descriptor_pool;

    bool m_is_right_mouse_pressed = false;
    window::ViewportPoint m_mouse_last_position = window::ViewportPoint();
    camera::Camera2D m_camera;

  public:
    ExamplePathing() {}

    ~ExamplePathing() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {

        auto window_size = ctx->window->get_framebuffer_size<float>();
        const float num_pixels_at_default_zoom = 200.0f;
        m_camera = camera::Camera2D(window_size.width, window_size.height,
                                    num_pixels_at_default_zoom);

        register_mouse_event_handler(ctx.get());

        m_swap_chain_manager = std::make_unique<vulkan::SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(
            ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT);
    }

    void register_mouse_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_mouse_event_callback(
            [this](window::MouseEvent mouse_event, window::ViewportPoint &point) -> void {
                switch (mouse_event) {
                case window::MouseEvent::RIGHT_BUTTON_DOWN:
                    m_is_right_mouse_pressed = true;
                    break;
                case window::MouseEvent::RIGHT_BUTTON_UP:
                    m_is_right_mouse_pressed = false;
                    break;
                case window::MouseEvent::CURSOR_MOVED:
                    if (m_is_right_mouse_pressed) {
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
                    logger::debug("mouse last position: ", m_mouse_last_position);
                    break;
                case window::MouseEvent::LEFT_BUTTON_UP:
                    break;
                }
            });
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        vulkan::RenderPass render_pass =
            m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        render_pass.end_submit_present();
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(960, 960),
                                              .title = "6_pathing"},
    };

    auto game = std::make_unique<ExamplePathing>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
