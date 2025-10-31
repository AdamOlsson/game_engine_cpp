#pragma once

#include "game_engine_sdk/render_engine/vulkan/Pipeline.h"
#include "game_engine_sdk/render_engine/vulkan/PipelineLayout.h"
#include "game_engine_sdk/render_engine/vulkan/buffers/IndexBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/buffers/VertexBuffer.h"
#include "vulkan/vulkan_core.h"

namespace graphics_pipeline {
std::vector<char> readFile(const std::string filename);

class GraphicsPipeline {
    friend class GraphicsPipelineBuilder;

  private:
    vulkan::Pipeline m_pipeline;
    vulkan::PipelineLayout m_pipeline_layout;

    GraphicsPipeline(vulkan::PipelineLayout &&layout, vulkan::Pipeline &&pipeline);

  public:
    GraphicsPipeline() = default;
    GraphicsPipeline(GraphicsPipeline &&other) noexcept = default;
    GraphicsPipeline &operator=(GraphicsPipeline &&other) noexcept = default;
    GraphicsPipeline(const GraphicsPipeline &other) = delete;
    GraphicsPipeline &operator=(const GraphicsPipeline &other) = delete;

    template <typename T>
    void bind_push_constants(const VkCommandBuffer &command_buffer,
                             VkShaderStageFlags stage_flags, T &values) {
        vkCmdPushConstants(command_buffer, m_pipeline_layout, stage_flags, 0,
                           sizeof(values), &values);
    }

    void render(const VkCommandBuffer &command_buffer,
                const vulkan::buffers::VertexBuffer &vertex_buffer,
                const vulkan::buffers::IndexBuffer &index_buffer,
                const VkDescriptorSet &descriptor_set, const size_t num_instances);
};
} // namespace graphics_pipeline
