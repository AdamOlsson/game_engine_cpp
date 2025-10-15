#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/render_engine/TilesetUVWT.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"
#include "game_engine_sdk/render_engine/window/WindowConfig.h"
#include <memory>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

enum class CellType : uint8_t {
    Wall,
    Grass,
};

class MapGeneration : public Game {
  private:
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;

    vulkan::Sampler m_sampler;
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;

    std::vector<graphics_pipeline::GeometryInstanceBufferObject> m_render_cells;

    Texture m_tileset;
    TilesetUVWT m_tileset_uvwt;

  public:
    MapGeneration() {

        const int noise_map_width = 4;
        const int noise_map_height = 3;
        const std::vector<float> noise_map = {0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
                                              1.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        auto rule = [](float value) -> CellType {
            if (value > 0.5) {
                return CellType::Wall;
            } else {
                return CellType::Grass;
            }
        };

        std::vector<CellType> cell_types;
        cell_types.reserve(noise_map.size());
        for (auto c : noise_map) {
            cell_types.push_back(rule(c));
        }

        std::vector<glm::vec4> cell_sprites;
        cell_sprites.reserve(noise_map.size());
        for (auto i = 0; i < cell_types.size(); i++) {
            cell_sprites.emplace_back(cell_types[i] == CellType::Grass ? colors::GREEN
                                                                       : colors::BLUE);
        }

        // For now let render data only be color, will eventually evolve to Wang tile algo
        const int cell_width = 20;
        const int cell_height = 20;
        const WorldPoint center_offset =
            WorldPoint((noise_map_width / 2.0f) * cell_width,
                       (noise_map_height / 2.0f) * cell_height);
        m_render_cells.reserve(cell_types.size());
        for (auto i = 0; i < cell_types.size(); i++) {
            const int x = (i % noise_map_width) * cell_width;
            const int y = (i / noise_map_width) * cell_height;
            m_render_cells.push_back(graphics_pipeline::GeometryInstanceBufferObject{
                .center = WorldPoint(x, y) - center_offset,
                .dimension = Dimension(cell_width, cell_height),
                .color = cell_sprites[i],
            });
        }
    }

    ~MapGeneration() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {
        m_swap_chain_manager = std::make_unique<SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<CommandBufferManager>(
            ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT);

        m_sampler = vulkan::Sampler(ctx, vulkan::Filter::NEAREST,
                                    vulkan::SamplerAddressMode::CLAMP_TO_BORDER);
        m_tileset = Texture::from_filepath(ctx, m_command_buffer_manager.get(),
                                           ASSET_FILE("forest_tileset_24x24.png"));
        m_tileset_uvwt = TilesetUVWT(m_tileset, TileSize(24, 24));

        m_geometry_pipeline = std::make_unique<graphics_pipeline::GeometryPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager,
            graphics_pipeline::GeometryPipelineOptions{
                .combined_image_samplers = {
                    vulkan::DescriptorImageInfo(m_tileset.view(), &m_sampler),
                }});
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        RenderPass render_pass = m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        auto &rectangle_instance_buffer =
            m_geometry_pipeline->get_rectangle_instance_buffer();

        rectangle_instance_buffer.clear();

        const size_t num_geometries = m_render_cells.size();
        for (auto i = 0; i < num_geometries; i++) {
            rectangle_instance_buffer.push_back(m_render_cells[i]);
        }

        rectangle_instance_buffer.transfer();

        m_geometry_pipeline->render_rectangles(command_buffer);

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
