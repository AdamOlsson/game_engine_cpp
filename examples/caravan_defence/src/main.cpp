#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "game_engine_sdk/render_engine/Texture.h"
#include "graphics_pipeline/quad/QuadPipeline.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/SwapChainManager.h"

class Guard {
  public:
    static constexpr float width = 50.0f;
    static constexpr float height = 50.0f;
    camera::WorldPoint2D position;

    math::Matrix model_matrix;

    // Render data
    static constexpr util::colors::Color color = util::colors::GREEN;

    Guard() = default;
    Guard(const camera::WorldPoint2D &position)
        : position(position),
          model_matrix(math::Matrix().scale(width, height, 1.0f).translate(position)) {}
    Guard(Guard &&other) noexcept = default;
    Guard(const Guard &other) = default;
    Guard &operator=(const Guard &other) = default;
    Guard &operator=(Guard &&other) noexcept = default;
    ~Guard() {}
};

class Caravan {
  public:
    static constexpr float width = 100.0f;
    static constexpr float height = 200.0f;
    camera::WorldPoint2D position;

    math::Matrix model_matrix;

    // Render data
    static constexpr util::colors::Color color = util::colors::MAGENTA;

    Caravan() = default;
    Caravan(const camera::WorldPoint2D &position)
        : position(position),
          model_matrix(math::Matrix().scale(width, height, 1.0f).translate(position)) {}
    Caravan(Caravan &&other) noexcept = default;
    Caravan(const Caravan &other) = default;
    Caravan &operator=(const Caravan &other) = default;
    Caravan &operator=(Caravan &&other) noexcept = default;
    ~Caravan() {}
};

class CaravanDefence : public Game {
  private:
    std::unique_ptr<vulkan::SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    camera::Camera2D m_camera;

    vulkan::DescriptorPool m_quad_pool;

    vulkan::Sampler m_sampler;
    Texture m_quad_texture;

    std::unique_ptr<
        vulkan::buffers::SwapStorageBuffer<graphics_pipeline::quad::QuadPipelineSBO>>
        m_quad_instances;
    std::unique_ptr<graphics_pipeline::quad::QuadPipelineDescriptorSet> m_quad_descriptor;
    std::unique_ptr<graphics_pipeline::quad::QuadPipeline> m_quad_pipeline;

    Caravan m_caravan;
    Guard m_guard_1;
    Guard m_guard_2;

  public:
    CaravanDefence() {}

    ~CaravanDefence() {};

    void update(float dt) override {};

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {

        auto window_size = ctx->window->get_framebuffer_size<float>();
        m_swap_chain_manager = std::make_unique<vulkan::SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        m_camera = camera::Camera2D(window_size.width, window_size.height);
        m_camera.set_zoom(0.1f);

        const unsigned int max_frames_in_flight = 2;

        m_sampler = vulkan::Sampler(ctx, vulkan::Filter::NEAREST,
                                    vulkan::SamplerAddressMode::CLAMP_TO_BORDER);
        m_quad_texture = Texture::empty(ctx, m_command_buffer_manager.get());

        vulkan::DescriptorPoolOpts pool_opts;
        pool_opts.max_num_descriptor_sets = max_frames_in_flight;
        pool_opts.num_storage_buffers = 1;
        pool_opts.num_uniform_buffers = 0;
        pool_opts.num_combined_image_samplers = 1;
        m_quad_pool = vulkan::DescriptorPool(ctx, std::move(pool_opts));

        m_quad_instances = std::make_unique<
            vulkan::buffers::SwapStorageBuffer<graphics_pipeline::quad::QuadPipelineSBO>>(
            ctx, max_frames_in_flight, 256);

        m_quad_descriptor =
            std::make_unique<graphics_pipeline::quad::QuadPipelineDescriptorSet>(
                ctx, m_quad_pool,
                graphics_pipeline::quad::QuadPipelineDescriptorSetOpts{
                    .storage_buffer_refs = vulkan::DescriptorBufferInfo::from_vector(
                        m_quad_instances->get_buffer_references()),
                    .combined_image_sampler_infos = {
                        vulkan::DescriptorImageInfo(m_quad_texture.view(), &m_sampler)}});

        vulkan::PushConstantRange push_constant_range;
        push_constant_range.offset = 0;
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        push_constant_range.size = camera::Camera2D::matrix_size();

        m_quad_pipeline = std::make_unique<graphics_pipeline::quad::QuadPipeline>(
            ctx, m_command_buffer_manager.get(), m_swap_chain_manager.get(),
            m_quad_descriptor->get_layout_handle(), &push_constant_range);

        // Add entities
        m_caravan = Caravan(camera::WorldPoint2D(0.0f, 0.0f));
        m_guard_1 = Guard(camera::WorldPoint2D(-3.0f, -4.0f));
        m_guard_2 = Guard(camera::WorldPoint2D(3.0f, -4.0f));

        // Fill buffers with entity render data
        m_quad_instances->push_back(graphics_pipeline::quad::QuadPipelineSBO{
            .model_matrix = m_caravan.model_matrix, .color = m_caravan.color});
        m_quad_instances->push_back(graphics_pipeline::quad::QuadPipelineSBO{
            .model_matrix = m_guard_1.model_matrix, .color = m_guard_1.color});
        m_quad_instances->push_back(graphics_pipeline::quad::QuadPipelineSBO{
            .model_matrix = m_guard_2.model_matrix, .color = m_guard_2.color});

        m_quad_instances->transfer();
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        vulkan::RenderPass render_pass =
            m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

        glm::mat4 push_constant = m_camera.get_view_projection_matrix();
        auto descriptor = m_quad_descriptor.get();
        const size_t num_instances = 3;
        m_quad_pipeline->render(command_buffer, descriptor, &push_constant,
                                num_instances);

        render_pass.end_submit_present();
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "Caravan Defence"},
    };

    auto game = std::make_unique<CaravanDefence>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
