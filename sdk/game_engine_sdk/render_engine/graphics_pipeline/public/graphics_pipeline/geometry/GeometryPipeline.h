#pragma once
#include "GeometryPipelineDecriptorSet.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/Pipeline.h"
#include "vulkan/PipelineLayout.h"
#include "vulkan/ShaderStage.h"
#include "vulkan/SwapChain.h"
#include "vulkan/context/GraphicsContext.h"
#include <memory>

namespace graphics_pipeline::geometry {

class GeometryPipeline {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    VkShaderStageFlags m_push_constant_stage;

    vulkan::PipelineLayout m_pipeline_layout;
    vulkan::Pipeline m_pipeline;

  public:
    GeometryPipeline() = default;
    GeometryPipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                     vulkan::CommandBufferManager *command_buffer_manager,
                     vulkan::SwapChain *swap_chain,
                     const vulkan::DescriptorSetLayout *descriptor_set_layout,
                     const vulkan::PushConstantRange *push_constant_range);

    VkPipeline get_pipeline() const { return m_pipeline; }
    VkPipelineLayout get_layout() const { return m_pipeline_layout; }
    vulkan::ShaderStageFlags get_push_constant_stage() const {
        return m_push_constant_stage;
    }

    // Convenience method to bind pipeline
    void bind(const vulkan::CommandBuffer &cmd) const {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    }
};

}; // namespace graphics_pipeline::geometry
