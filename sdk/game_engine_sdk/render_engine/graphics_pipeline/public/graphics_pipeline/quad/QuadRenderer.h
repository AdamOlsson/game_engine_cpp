#pragma once

#include "graphics_pipeline/quad/QuadPipeline.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "vulkan/SwapChainManager.h"
#include "vulkan/buffers/GpuBuffer.h"
namespace graphics_pipeline::quad {

class QuadRenderer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    QuadPipeline m_quad_pipeline;

    vulkan::buffers::VertexBuffer<vulkan::Vertex> m_quad_vertex_buffer;
    vulkan::buffers::IndexBuffer m_quad_index_buffer;

    /*vulkan::DescriptorPool m_descriptor_pool;*/
    /*vulkan::Sampler m_sampler;*/
    /*SwapDescriptorSet m_descriptor_sets;*/

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

  public:
    vulkan::buffers::StorageBuffer<QuadPipelineSBO> m_instances;

    QuadRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                 vulkan::CommandBufferManager *command_buffer_manager,
                 vulkan::SwapChainManager *swap_chain_manager,
                 const VkPushConstantRange *push_constant_range,
                 const vulkan::DescriptorSetLayout &layout);

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer,
                QuadPipelineDescriptorSet *descriptor_set,
                PushConstantType *push_constant, const int num_instances) {

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_quad_pipeline.get_layout(),
                               m_quad_pipeline.get_push_constant_stage(), 0,
                               sizeof(*push_constant), push_constant);
        }

        if (descriptor_set) {
            // TODO: Handle descriptor set
            const vulkan::DescriptorSet set = descriptor_set->get_current();
            vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_quad_pipeline.get_layout(), 0, 1, &set, 0, nullptr);
        }

        m_quad_pipeline.bind(command_buffer);

        const VkDeviceSize vertex_buffers_offset = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_quad_vertex_buffer.buffer,
                               &vertex_buffers_offset);
        vkCmdBindIndexBuffer(command_buffer, m_quad_index_buffer.buffer, 0,
                             VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command_buffer, m_quad_index_buffer.num_indices, num_instances,
                         0, 0, 0);
    }

    /*QuadPipelineSBO *request_render_slot() {*/
    /*    QuadPipelineSBO *slot = m_render_slot_cache.top();*/
    /*    m_render_slot_cache.pop();*/
    /*    return slot;*/
    /*}*/
};
} // namespace graphics_pipeline::quad
