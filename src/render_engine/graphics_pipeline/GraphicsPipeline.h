#pragma once

#include "render_engine/Pipeline.h"
#include "render_engine/buffers/IndexBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
#include "vulkan/vulkan_core.h"

namespace graphics_pipeline {
class GraphicsPipeline {
    friend class GraphicsPipelineBuilder;

  private:
    Pipeline m_pipeline;

    GraphicsPipeline(Pipeline &&pipeline);

  public:
    GraphicsPipeline(GraphicsPipeline &&other) noexcept = default;
    GraphicsPipeline &operator=(GraphicsPipeline &&other) noexcept = default;
    GraphicsPipeline(const GraphicsPipeline &other) = delete;
    GraphicsPipeline &operator=(const GraphicsPipeline &other) = delete;

    template <typename T>
    void bind_push_constants(const VkCommandBuffer &command_buffer,
                             VkShaderStageFlags stage_flags, T values) {
        vkCmdPushConstants(command_buffer, m_pipeline.m_pipeline_layout, stage_flags, 0,
                           sizeof(values), &values);
    }

    void render(const VkCommandBuffer &command_buffer, const VertexBuffer &vertex_buffer,
                const IndexBuffer &index_buffer, const VkDescriptorSet &descriptor_set,
                const size_t num_instances);
};
} // namespace graphics_pipeline
