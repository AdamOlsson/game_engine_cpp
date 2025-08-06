#pragma once

#include "PipelineLayout.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace graphics_pipeline {
class Pipeline {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    VkPipeline m_pipeline;

    VkPipeline create_graphics_pipeline(PipelineLayout *layout,
                                        const VkShaderModule vertex_shader_module,
                                        const VkShaderModule fragment_shader_module,
                                        SwapChainManager &swap_chain_manager);

  public:
    Pipeline() = default;
    Pipeline(std::shared_ptr<graphics_context::GraphicsContext> ctx,
             PipelineLayout *layout, const VkShaderModule vertex_shader_module,
             const VkShaderModule fragment_shader_module,
             SwapChainManager &swap_chain_manager);

    ~Pipeline();

    Pipeline(Pipeline &&other) noexcept;
    Pipeline &operator=(Pipeline &&other) noexcept;

    Pipeline &operator=(const Pipeline &other) = delete;
    Pipeline(const Pipeline &other) = delete;

    operator VkPipeline() const { return m_pipeline; }
};
} // namespace graphics_pipeline
