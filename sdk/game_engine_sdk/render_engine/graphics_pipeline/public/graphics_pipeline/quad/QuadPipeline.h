#pragma once
#include "QuadPipelineDescriptorSet.h"
#include "vulkan/CommandBuffer.h"
#include "vulkan/Pipeline.h"
#include "vulkan/PipelineLayout.h"
#include "vulkan/PushConstantRange.h"
#include "vulkan/ShaderStage.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include "vulkan/context/GraphicsContext.h"
#include <memory>

namespace graphics_pipeline::quad {

class QuadPipeline {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    vulkan::buffers::VertexBuffer m_quad_vertex_buffer;
    vulkan::buffers::IndexBuffer m_quad_index_buffer;

    vulkan::ShaderStageFlags m_push_constant_stage;

    vulkan::PipelineLayout m_pipeline_layout;
    vulkan::Pipeline m_pipeline;

    static constexpr std::array<uint16_t, 6> m_quad_indices = {0, 1, 2, 0, 2, 3};
    static constexpr const std::array<Vertex, 4> m_quad_vertices = {
        Vertex(-0.5f, -0.5f, 0.0f),
        Vertex(-0.5f, 0.5f, 0.0f),
        Vertex(0.5f, 0.5f, 0.0f),
        Vertex(0.5f, -0.5f, 0.0f),
    };

  public:
    QuadPipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 vulkan::CommandBufferManager *command_buffer_manager,
                 vulkan::SwapChainManager *swap_chain_manager,
                 const vulkan::DescriptorSetLayout *descriptor_set_layout,
                 const vulkan::PushConstantRange *push_constant_range);

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer,
                QuadPipelineDescriptorSet *descriptor_set,
                PushConstantType *push_constant, const int num_instances) {

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_pipeline_layout, m_push_constant_stage,
                               0, sizeof(*push_constant), push_constant);
        }

        if (descriptor_set) {
            const vulkan::DescriptorSet set = descriptor_set->get_next();
            vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_pipeline_layout, 0, 1, &set, 0, nullptr);
        }

        const VkDeviceSize vertex_buffers_offset = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_quad_vertex_buffer.buffer,
                               &vertex_buffers_offset);
        vkCmdBindIndexBuffer(command_buffer, m_quad_index_buffer.buffer, 0,
                             VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command_buffer, m_quad_index_buffer.num_indices, num_instances,
                         0, 0, 0);
    }
};

} // namespace graphics_pipeline::quad
