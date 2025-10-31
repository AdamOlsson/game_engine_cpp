#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/io.h"
#include "game_engine_sdk/render_engine/Camera.h"
#include "game_engine_sdk/render_engine/ModelMatrix.h"
#include "game_engine_sdk/render_engine/TilesetUVWT.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipeline.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipelineSBO.h"
#include "game_engine_sdk/render_engine/window/WindowConfig.h"
#include "tiles.h"
#include "tiling/NoiseMap.h"
#include "tiling/wang/WangTiles.h"
#include "vulkan/vulkan_core.h"
#include <memory>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

// CONTINUE: Render Wang tiling
// - Look into the artifacts and how to solve them
// - Make rendering agnostic to window size
// - Fix import path to prefix with "game_engine_sdk" for modules
// - Center tiles on screen

using namespace tiling;

class MapGeneration : public Game {
  private:
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;

    vulkan::Sampler m_sampler;

    vulkan::DescriptorPool m_descriptor_pool;
    std::unique_ptr<SwapStorageBuffer<graphics_pipeline::QuadPipelineSBO>>
        m_quad_storage_buffer;
    std::unique_ptr<graphics_pipeline::QuadPipelineDescriptorSet> m_quad_descriptor_set;
    std::unique_ptr<graphics_pipeline::QuadPipeline> m_quad_pipeline;
    size_t m_num_instances;

    std::vector<graphics_pipeline::GeometryInstanceBufferObject> m_render_cells;

    Texture m_tileset;
    TilesetUVWT m_tileset_uvwt;

    bool m_is_right_mouse_pressed = false;
    window::ViewportPoint m_mouse_last_position = window::ViewportPoint();
    Camera2D m_camera;

    wang::WangTiles<CellType> m_wang_tiles;

  public:
    MapGeneration() {}

    ~MapGeneration() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {

        auto tileset_constraints = create_tileset_constraints();
        auto noise_map =
            tiling::NoiseMap::unique_from_filepath(ASSET_FILE("noise_map.jpeg"));

        auto rule = [](float value) -> CellType {
            if (value > 0.5) {
                return CellType::Wall;
            } else {
                return CellType::Grass;
            }
        };

        m_wang_tiles = wang::WangTiles<CellType>(*noise_map, std::move(rule),
                                                 std::move(tileset_constraints));

        auto window_size = ctx->window->get_framebuffer_size<float>();
        const float num_pixels_at_default_zoom = 200.0f;
        m_camera =
            Camera2D(window_size.width, window_size.height, num_pixels_at_default_zoom);

        register_mouse_event_handler(ctx.get());

        m_swap_chain_manager = std::make_unique<SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<CommandBufferManager>(
            ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT);

        m_sampler = vulkan::Sampler(ctx, vulkan::Filter::NEAREST,
                                    vulkan::SamplerAddressMode::CLAMP_TO_BORDER);
        m_tileset = Texture::from_filepath(ctx, m_command_buffer_manager.get(),
                                           ASSET_FILE("forest_tileset_24x24.png"));
        m_tileset_uvwt = TilesetUVWT(m_tileset, TileSize(24, 24));

        m_quad_storage_buffer =
            std::make_unique<SwapStorageBuffer<graphics_pipeline::QuadPipelineSBO>>(
                ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT,
                m_wang_tiles.width() * m_wang_tiles.height());

        m_descriptor_pool = vulkan::DescriptorPool(
            ctx, vulkan::DescriptorPoolOpts{.max_num_descriptor_sets =
                                                graphics_pipeline::MAX_FRAMES_IN_FLIGHT,
                                            .num_storage_buffers = 1,
                                            .num_uniform_buffers = 0,
                                            .num_combined_image_samplers = 1});

        m_quad_descriptor_set =
            std::make_unique<graphics_pipeline::QuadPipelineDescriptorSet>(
                ctx, m_descriptor_pool,
                graphics_pipeline::QuadPipelineDescriptorSetOpts{
                    .storage_buffer_refs = vulkan::DescriptorBufferInfo::from_vector(
                        m_quad_storage_buffer->get_buffer_references()),
                    .combined_image_sampler_infos = {
                        vulkan::DescriptorImageInfo(m_tileset.view(), &m_sampler)}});
        auto &quad_descriptor_set_layout = m_quad_descriptor_set->get_layout();
        auto quad_push_constant_range =
            vulkan::PushConstantRange{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                      .offset = 0,
                                      .size = Camera2D::matrix_size()};
        m_quad_pipeline = std::make_unique<graphics_pipeline::QuadPipeline>(
            ctx, m_command_buffer_manager.get(), m_swap_chain_manager.get(),
            &quad_descriptor_set_layout, &quad_push_constant_range);

        const auto cell_size = glm::vec2(24.0f, 24.0f);
        auto base_model_matrix = ModelMatrix().scale(cell_size.x, cell_size.y, 1.0f);
        const auto num_tiles = m_wang_tiles.width() * m_wang_tiles.height();
        m_num_instances = 0;
        for (auto i = 0; i < num_tiles; i++) {
            const int x = i % m_wang_tiles.width();
            const int y = i / m_wang_tiles.width();
            const auto tileset_index = m_wang_tiles.lookup_tile(x, y);

            const auto uvwt =
                tileset_index.has_value()
                    ? m_tileset_uvwt.uvwt_for_tile_at(tileset_index->x, tileset_index->y)
                    : m_tileset_uvwt.uvwt_for_tile_at(0, 0);
            m_quad_storage_buffer->write(graphics_pipeline::QuadPipelineSBO{
                .model_matrix = ModelMatrix(base_model_matrix).translate(x, y, 0),
                .uvwt = uvwt,
            });
            m_num_instances++;
        }
    }

    void register_mouse_event_handler(graphics_context::GraphicsContext *ctx) {
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
                        m_camera.set_relative_position(world_delta);
                    }
                    m_mouse_last_position = point;
                    break;
                case window::MouseEvent::SCROLL:
                    m_camera.set_relative_zoom(point.y);
                    break;
                case window::MouseEvent::LEFT_BUTTON_DOWN:
                case window::MouseEvent::LEFT_BUTTON_UP:
                    break;
                }
            });
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        RenderPass render_pass = m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        auto descriptor = m_quad_descriptor_set.get();
        glm::mat4 push_constant = m_camera.get_view_projection_matrix();
        m_quad_pipeline->render(command_buffer, descriptor, &push_constant,
                                m_num_instances);

        render_pass.end_submit_present();
    };
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "5_map_generation"},
    };

    auto game = std::make_unique<MapGeneration>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
