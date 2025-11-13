#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/render_engine/ModelMatrix.h"
#include "graphics_pipeline/geometry/GeometryPipeline.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "vulkan/DescriptorPool.h"
/*#include "vulkan/buffers/GpuBuffer.h.h"*/
#include "window/WindowConfig.h"
#include <memory>

// CONTINUE:
// - Render the grid using new geometry shader
//      - Get the rectangle dimension to the fragment shader (maybe through the model
//      matrix)
//      - fix such the hovering over a square highlights it
// - Partial updates of the storage buffer

constexpr size_t TILE_SIZE = 24; // World space
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

    std::unique_ptr<vulkan::buffers::SwapStorageBuffer<
        graphics_pipeline::geometry::GeometryPipelineSBO>>
        m_tile_instances;
    size_t m_num_instances;
    std::unique_ptr<graphics_pipeline::geometry::GeometryPipelineDescriptorSet>
        m_descriptor_set;
    std::unique_ptr<graphics_pipeline::geometry::GeometryPipeline> m_pipeline;

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
        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        m_descriptor_pool = vulkan::DescriptorPool(
            ctx, vulkan::DescriptorPoolOpts{.max_num_descriptor_sets = 2,
                                            .num_storage_buffers = 1,
                                            .num_uniform_buffers = 0,
                                            .num_combined_image_samplers = 0});

        const auto window_dims = ctx->window->dimensions<size_t>();
        // multiply by 2 is simply to cover the entire screen and more
        auto num_tiles_width = (window_dims.width / TILE_SIZE) * 2;
        auto num_tiles_height = (window_dims.width / TILE_SIZE) * 2;
        m_tile_instances = std::make_unique<vulkan::buffers::SwapStorageBuffer<
            graphics_pipeline::geometry::GeometryPipelineSBO>>(
            ctx, 2, num_tiles_width * num_tiles_height);

        m_descriptor_set =
            std::make_unique<graphics_pipeline::geometry::GeometryPipelineDescriptorSet>(
                ctx, m_descriptor_pool,
                graphics_pipeline::geometry::GeometryPipelineDescriptorSetOpts{
                    .storage_buffer_refs = vulkan::DescriptorBufferInfo::from_vector(
                        m_tile_instances->get_buffer_references())});

        auto &descriptor_layout = m_descriptor_set->get_layout();
        auto quad_push_constant_range =
            vulkan::PushConstantRange{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                      .offset = 0,
                                      .size = camera::Camera2D::matrix_size()};

        m_pipeline = std::make_unique<graphics_pipeline::geometry::GeometryPipeline>(
            ctx, m_command_buffer_manager.get(), m_swap_chain_manager.get(),
            &descriptor_layout, &quad_push_constant_range);

        for (auto i = 0; i < 2; i++) {
            m_tile_instances->push_back(graphics_pipeline::geometry::GeometryPipelineSBO{
                .model_matrix =
                    ModelMatrix().scale(TILE_SIZE, TILE_SIZE, 1.0f).translate(i, 0, 0),
                .border =
                    {
                        .color = util::colors::RED,
                        .width = 2,
                    },
            });
            m_num_instances++;
        }
        m_tile_instances->transfer();
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

        if (abs(m_mouse_last_position.x) < 24.0 && abs(m_mouse_last_position.y) < 24.0) {
            auto &instance_buffer = m_tile_instances->get_buffer();
            instance_buffer[0].color = util::colors::BLUE;
            instance_buffer.transfer();
        } else {
            auto &instance_buffer = m_tile_instances->get_buffer();
            instance_buffer[0].color = util::colors::TRANSPARENT;
            instance_buffer.transfer();
        }

        auto descriptor = m_descriptor_set.get();
        glm::mat4 push_constant = m_camera.get_view_projection_matrix();
        m_pipeline->render(command_buffer, descriptor, &push_constant, m_num_instances);

        render_pass.end_submit_present();

        m_tile_instances->rotate();
        m_descriptor_set->rotate();
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(1920, 960),
                                              .title = "6_pathing"},
    };

    auto game = std::make_unique<ExamplePathing>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
