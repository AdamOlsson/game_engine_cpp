#pragma once

#include "render_engine/SwapChainManager.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

namespace graphics_pipeline {
class Pipeline {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    VkPipelineLayout create_graphics_pipeline_layout(
        const VkDescriptorSetLayout &descriptor_set_layout,
        const std::vector<VkPushConstantRange> &push_constant_range);

    VkPipeline create_graphics_pipeline(const VkShaderModule vertex_shader_module,
                                        const VkShaderModule fragment_shader_module,
                                        SwapChainManager &swap_chain_manager);

  public:
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_pipeline;

    Pipeline() = default;
    Pipeline(std::shared_ptr<graphics_context::GraphicsContext> ctx,
             const VkDescriptorSetLayout &descriptor_set_layout,
             const std::vector<VkPushConstantRange> &push_constant_range,
             const VkShaderModule vertex_shader_module,
             const VkShaderModule fragment_shader_module,
             SwapChainManager &swap_chain_manager);

    ~Pipeline();

    Pipeline(Pipeline &&other) noexcept;
    Pipeline &operator=(Pipeline &&other) noexcept;

    Pipeline &operator=(const Pipeline &other) = delete;
    Pipeline(const Pipeline &other) = delete;
};
} // namespace graphics_pipeline
