#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/SwapChainManager.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class Pipeline {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    VkPipelineLayout
    create_graphics_pipeline_layout(VkDescriptorSetLayout &descriptorSetLayout);

    VkPipeline create_graphics_pipeline(const VkShaderModule vertShaderModule,
                                        const VkShaderModule fragShaderModule,
                                        SwapChainManager &swap_chain_manager);

  public:
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_pipeline;

    Pipeline() = default;
    Pipeline(std::shared_ptr<CoreGraphicsContext> ctx,
             VkDescriptorSetLayout &descriptor_set_layout,
             const VkShaderModule vertex_shader_module,
             const VkShaderModule fragment_shader_module,
             SwapChainManager &swap_chain_manager);

    ~Pipeline();
    Pipeline(const Pipeline &other) = delete;
    Pipeline(Pipeline &&other) noexcept = default;

    Pipeline &operator=(const Pipeline &other) = delete;
    Pipeline &operator=(Pipeline &&other) noexcept = default;
};
