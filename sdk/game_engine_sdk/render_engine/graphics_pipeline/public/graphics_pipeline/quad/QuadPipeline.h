#pragma once
#include "graphics_pipeline/PipelineOpts.h"
#include "vulkan/CommandBuffer.h"
#include "vulkan/Pipeline.h"
#include "vulkan/PipelineLayout.h"
#include "vulkan/PushConstantRange.h"
#include "vulkan/ShaderStage.h"
#include "vulkan/context/GraphicsContext.h"
#include <memory>

namespace graphics_pipeline::quad {

class QuadPipeline {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    vulkan::PushConstantRange m_push_constant;

    vulkan::PipelineLayout m_pipeline_layout;
    vulkan::Pipeline m_pipeline;

  public:
    QuadPipeline() = default;
    QuadPipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 graphics_pipeline::PipelineOpts &opts);

    VkPipeline get_pipeline() const { return m_pipeline; }
    VkPipelineLayout get_layout() const { return m_pipeline_layout; }
    vulkan::ShaderStageFlags get_push_constant_stage() const {
        return m_push_constant.stageFlags;
    }

    // Convenience method to bind pipeline
    void bind(const vulkan::CommandBuffer &cmd) const {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    }
};

} // namespace graphics_pipeline::quad
