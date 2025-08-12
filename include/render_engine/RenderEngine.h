#pragma once
#include "render_engine/RenderBody.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/fonts/Font.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "render_engine/graphics_pipeline/GeometryPipeline.h"
#include "render_engine/graphics_pipeline/TextPipeline.h"
#include "render_engine/graphics_pipeline/UIPipeline.h"
#include "render_engine/ui/State.h"
#include <GLFW/glfw3.h>
#include <memory>

class RenderEngine {
  private:
    vulkan::Sampler m_sampler;

    std::unique_ptr<Texture> m_dog_image;
    std::unique_ptr<graphics_pipeline::GeometryPipeline> m_geometry_pipeline;
    std::unique_ptr<graphics_pipeline::TextPipeline> m_text_pipeline;
    std::unique_ptr<graphics_pipeline::UIPipeline> m_ui_pipeline;

  public:
    RenderEngine(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 CommandBufferManager *command_buffer_manager,
                 SwapChainManager *swap_chain_manager, const UseFont use_font);
    ~RenderEngine();

    void render(vulkan::CommandBuffer &command_buffer,
                const std::vector<std::reference_wrapper<const RenderBody>> &bodies);

    void render_text(vulkan::CommandBuffer &command_buffer, const ui::TextBox &text_box);

    void render_ui(vulkan::CommandBuffer &command_buffer, const ui::State &state);
};
