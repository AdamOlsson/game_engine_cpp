#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/render_engine/TilesetUVWT.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "math/Matrix.h"
#include "tiles.h"
#include "tiling/NoiseMap.h"
#include "tiling/TileGrid.h"
#include "tiling/wang/WangTiles.h"
#include "vulkan/SwapChain.h"
#include "vulkan/vulkan_core.h"
#include "window/WindowConfig.h"
#include <memory>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

using namespace tiling;

constexpr float CELL_SIZE = 24.0f;
constexpr float ZOOM_SCALE_FACTOR = 0.1f;
constexpr glm::vec2 INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr glm::vec2 INVERT_X_AXIS = glm::vec2(-1.0f, 1.0f);
constexpr glm::vec2 INVERT_Y_AXIS = glm::vec2(1.0f, -1.0f);
constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

class MapGeneration : public Game {
  private:
    std::unique_ptr<vulkan::SwapChain> m_swap_chain;
    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    vulkan::Sampler m_sampler;

    std::unique_ptr<graphics_pipeline::quad::QuadRenderer> m_quad_renderer;
    std::vector<graphics_pipeline::quad::QuadSBOHandle> m_handles;

    size_t m_num_instances;

    TilesetUVWT m_tileset_uvwt;

    bool m_is_right_mouse_pressed = false;
    interface::ViewportPoint m_mouse_last_position;
    camera::Camera2D m_camera;

  public:
    MapGeneration() {}

    ~MapGeneration() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {

        auto tileset_constraints = create_tileset_constraints();
        auto noise_map =
            tiling::NoiseMap::unique_from_filepath(ASSET_FILE("noise_map.jpeg"));

        auto grid_assign_rule = [](float value) -> tiling::Tile<CellType> {
            if (value > 0.5) {
                return tiling::Tile{
                    .type = CellType::Wall,
                    .weight = 9999.0f,
                };
            } else {
                return tiling::Tile{
                    .type = CellType::Grass,
                    .weight = 1.0f,
                };
            }
        };

        auto grid = tiling::TileGrid<CellType>(noise_map->width, noise_map->height);
        for (auto i = 0; i < noise_map->width * noise_map->height; i++) {
            grid[i] = grid_assign_rule(noise_map->noise[i]);
        }

        auto window_size = ctx->window->get_framebuffer_size<float>();
        const float num_pixels_at_default_zoom = 200.0f;
        m_camera = camera::Camera2D(window_size.width, window_size.height,
                                    num_pixels_at_default_zoom);

        register_mouse_event_handler(ctx.get());

        m_swap_chain = std::make_unique<vulkan::SwapChain>(ctx);
        m_command_buffer_manager =
            std::make_unique<vulkan::CommandBufferManager>(ctx, MAX_FRAMES_IN_FLIGHT);

        m_sampler = vulkan::Sampler(ctx, vulkan::Filter::NEAREST,
                                    vulkan::SamplerAddressMode::CLAMP_TO_BORDER);
        auto tileset = graphics_pipeline::Texture::from_filepath(
            ctx, m_command_buffer_manager.get(), ASSET_FILE("forest_tileset_24x24.png"));
        m_tileset_uvwt = TilesetUVWT(tileset, TileSize(24, 24));

        auto quad_push_constant_range =
            vulkan::PushConstantRange{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                      .offset = 0,
                                      .size = camera::Camera2D::matrix_size()};

        graphics_pipeline::RendererOpts renderer_opts{};
        renderer_opts.push_constant_range = quad_push_constant_range;
        renderer_opts.swap_chain.extent = m_swap_chain->get_extent();
        renderer_opts.swap_chain.render_pass = m_swap_chain->get_render_pass_handle();
        renderer_opts.quad.texture = std::move(tileset);
        renderer_opts.quad.instance_buffer_opts.size = grid.width() * grid.height();
        m_quad_renderer = std::make_unique<graphics_pipeline::quad::QuadRenderer>(
            ctx, m_command_buffer_manager.get(), renderer_opts);

        const auto num_tiles = grid.width() * grid.height();
        m_num_instances = 0;
        for (auto i = 0; i < num_tiles; i++) {
            const int x = i % grid.width();
            const int y = i / grid.width();

            const auto tileset_index =
                tiling::wang::lookup_tile_sprite(grid, tileset_constraints, x, y);

            const glm::vec4 uvwt =
                tileset_index.has_value()
                    ? m_tileset_uvwt.uvwt_for_tile_at(tileset_index->x, tileset_index->y)
                    : m_tileset_uvwt.uvwt_for_tile_at(0, 0);

            graphics_pipeline::quad::QuadSBOHandle handle =
                m_quad_renderer->request_render_slot();
            graphics_pipeline::quad::QuadPipelineSBO &instance =
                m_quad_renderer->get_instance(handle);
            instance.model_matrix = math::Matrix()
                                        .scale(glm::vec3(CELL_SIZE, CELL_SIZE, 1.0))
                                        .translate(x, y, 0);
            instance.uvwt = uvwt;
            m_handles.push_back(std::move(handle));

            m_num_instances++;
        }
        m_quad_renderer->sync_render_slots();
    }

    void register_mouse_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_mouse_event_callback(
            [this](window::MouseEvent mouse_event,
                   interface::ViewportPoint &point) -> void {
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
                    m_camera.set_relative_zoom(point.y() * ZOOM_SCALE_FACTOR);

                    break;
                case window::MouseEvent::LEFT_BUTTON_DOWN:
                    break;
                case window::MouseEvent::LEFT_BUTTON_UP:
                    break;
                }
            });
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        vulkan::Frame frame = m_swap_chain->begin_frame(command_buffer);

        glm::mat4 push_constant = m_camera.get_view_projection_matrix();

        frame.begin_render_pass();
        m_quad_renderer->render(command_buffer, &push_constant, m_num_instances);
        frame.end_render_pass();

        frame.submit_present();
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(1080, 960),
                                              .title = "5_map_generation"},
    };

    auto game = std::make_unique<MapGeneration>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
