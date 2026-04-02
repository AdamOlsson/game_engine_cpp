#pragma once

#include "graphics_pipeline/RendererOpts.h"
#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/Texture.h"
#include "graphics_pipeline/quad/QuadPipeline.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/DrawIndexedIndirectCommand.h"
#include "vulkan/Sampler.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include <memory>

namespace graphics_pipeline::quad {

class QuadRenderer2 {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    QuadPipeline m_quad_pipeline;

    vulkan::buffers::VertexBuffer<vulkan::Vertex> m_quad_vertex_buffer;
    vulkan::buffers::IndexBuffer m_quad_index_buffer;

    vulkan::DescriptorPool m_descriptor_pool;

    std::vector<Texture> m_textures;
    vulkan::Sampler m_sampler;

    SwapDescriptorSet m_descriptor_sets;

    vulkan::buffers::StorageBuffer<QuadPipelineSBO> m_instances;
    vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand> m_draw_commands;

    vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                              const size_t num_textures);

  public:
    QuadRenderer2(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                  vulkan::CommandBufferManager *command_buffer_manager,
                  RendererOpts &opts);

    QuadRenderer2(QuadRenderer2 &&) noexcept = delete;
    QuadRenderer2 &operator=(QuadRenderer2 &&) noexcept = delete;

    QuadRenderer2(const QuadRenderer2 &) = delete;
    QuadRenderer2 &operator=(const QuadRenderer2 &) = delete;

    vulkan::DrawIndexedIndirectCommand
    write_to_buffer(const std::vector<QuadPipelineSBO> &instance_data,
                    const size_t offset = 0);

    template <typename PushConstantType>
    void render_indirect(
        const vulkan::CommandBuffer &command_buffer, PushConstantType *push_constant,
        const std::vector<vulkan::DrawIndexedIndirectCommand> &draw_commands) {

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_quad_pipeline.get_layout(),
                               m_quad_pipeline.get_push_constant_stage(), 0,
                               sizeof(*push_constant), push_constant);
        }

        const vulkan::DescriptorSet set = m_descriptor_sets.get_current();
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_quad_pipeline.get_layout(), 0, 1, &set, 0, nullptr);

        m_quad_pipeline.bind(command_buffer);

        const VkDeviceSize vertex_buffers_offset = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_quad_vertex_buffer.buffer,
                               &vertex_buffers_offset);
        vkCmdBindIndexBuffer(command_buffer, m_quad_index_buffer.buffer, 0,
                             VK_INDEX_TYPE_UINT16);

        m_draw_commands.write(draw_commands);

        vkCmdDrawIndexedIndirect(command_buffer, m_draw_commands.handle(), 0,
                                 draw_commands.size(),
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
};

} // namespace graphics_pipeline::quad
