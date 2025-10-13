#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"
#include "game_engine_sdk/render_engine/window/WindowConfig.h"
#include <memory>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

class MapGeneration : public Game {
  private:
    std::unique_ptr<SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<CommandBufferManager> m_command_buffer_manager;

    vulkan::Sampler m_sampler;
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;
    std::vector<graphics_pipeline::GeometryInstanceBufferObject> m_geometries;

  public:
    MapGeneration() {}

    ~MapGeneration() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<graphics_context::GraphicsContext> &ctx) override {
        m_swap_chain_manager = std::make_unique<SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<CommandBufferManager>(
            ctx, graphics_pipeline::MAX_FRAMES_IN_FLIGHT);

        m_sampler = vulkan::Sampler(ctx, vulkan::Filter::NEAREST,
                                    vulkan::SamplerAddressMode::CLAMP_TO_BORDER);

        m_geometry_pipeline = std::make_unique<graphics_pipeline::GeometryPipeline>(
            ctx, m_command_buffer_manager.get(), *m_swap_chain_manager,
            graphics_pipeline::GeometryPipelineOptions{.combined_image_samplers = {}});
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        RenderPass render_pass = m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        auto &rectangle_instance_buffer =
            m_geometry_pipeline->get_rectangle_instance_buffer();

        rectangle_instance_buffer.clear();

        const size_t num_geometries = m_geometries.size();
        for (auto i = 0; i < num_geometries; i++) {
            rectangle_instance_buffer.push_back(m_geometries[i]);
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
