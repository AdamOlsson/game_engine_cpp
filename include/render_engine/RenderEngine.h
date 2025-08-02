#pragma once
#include "render_engine/GeometryPipeline.h"
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/buffers/GpuBuffer.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "render_engine/ui/State.h"
#include "render_engine/ui/TextPipeline.h"
#include "render_engine/ui/UIPipeline.h"
#include <GLFW/glfw3.h>
#include <memory>

class RenderEngine {
  private:
    bool framebuffer_resized = false;

    SwapUniformBuffer<window::WindowDimension<float>> m_window_dimension_buffers;

    Sampler m_sampler;
    std::unique_ptr<Texture> m_texture; // Having this unique prevents a segfault

    std::unique_ptr<GeometryPipeline> m_geometry_pipeline;

    std::unique_ptr<ui::TextPipeline> m_text_pipeline;
    std::unique_ptr<ui::UIPipeline> m_ui_pipeline;

    struct {
        CommandBuffer command_buffer;
    } m_current_render_pass;

  public:
    RenderEngine(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 SwapChainManager *swap_chain_manager, const UseFont use_font);
    ~RenderEngine();

    void render(CommandBuffer &command_buffer,
                const std::vector<std::reference_wrapper<const RenderBody>> &bodies);

    void render_text(const ui::TextBox &text_box);

    void render_ui(CommandBuffer &command_buffer, const ui::State &state);

    bool begin_render_pass(SwapChainManager *swap_chain_manager,
                           graphics_context::DeviceQueues &m_device_queues);

    bool end_render_pass(SwapChainManager *swap_chain_manager,
                         graphics_context::DeviceQueues &m_device_queues);
};
