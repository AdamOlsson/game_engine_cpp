#pragma once

#include "graphics_pipeline/RendererOpts.h"
#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/geometry/GeometryPipeline.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/DrawIndexedIndirectCommand.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include <memory>

namespace graphics_pipeline::geometry {

class GeometrySBOHandle;

class GeometryRenderer2 {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    vulkan::buffers::VertexBuffer<vulkan::Vertex> m_quad_vertex_buffer;
    vulkan::buffers::IndexBuffer m_quad_index_buffer;

    std::unique_ptr<GeometryPipeline> m_geometry_pipeline;

    vulkan::DescriptorPool m_descriptor_pool;

    SwapDescriptorSet m_descriptor_sets;

    vulkan::buffers::StorageBuffer<GeometryPipelineSBO> m_instances;
    vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand> m_draw_commands;

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

  public:
    GeometryRenderer2(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                      vulkan::CommandBufferManager *command_buffer_manager,
                      RendererOpts &opts);

    GeometryRenderer2(GeometryRenderer2 &&) noexcept = delete;
    GeometryRenderer2 &operator=(GeometryRenderer2 &&) noexcept = delete;

    GeometryRenderer2(const GeometryRenderer2 &) = delete;
    GeometryRenderer2 &operator=(const GeometryRenderer2 &) = delete;

    vulkan::DrawIndexedIndirectCommand
    write_to_buffer_indexed(const std::vector<GeometryPipelineSBO> &instance_data,
                            const std::vector<size_t> &indices, const size_t offset = 0);

    vulkan::DrawIndexedIndirectCommand
    write_to_buffer(const std::vector<GeometryPipelineSBO> &instance_data,
                    const size_t offset = 0);

    template <typename PushConstantType>
    void render_indirect(
        const vulkan::CommandBuffer &command_buffer, PushConstantType *push_constant,
        const std::vector<vulkan::DrawIndexedIndirectCommand> &draw_commands) {

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_geometry_pipeline->get_layout(),
                               m_geometry_pipeline->get_push_constant_stage(), 0,
                               sizeof(*push_constant), push_constant);
        }

        // Handle descriptor set
        const vulkan::DescriptorSet set = m_descriptor_sets.get_current();
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_geometry_pipeline->get_layout(), 0, 1, &set, 0,
                                nullptr);

        m_geometry_pipeline->bind(command_buffer);

        const VkDeviceSize vertex_buffers_offset = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_quad_vertex_buffer.buffer,
                               &vertex_buffers_offset);
        vkCmdBindIndexBuffer(command_buffer, m_quad_index_buffer.buffer, 0,
                             VK_INDEX_TYPE_UINT16);

        m_draw_commands.write(draw_commands);

        vkCmdDrawIndexedIndirect(command_buffer, m_draw_commands.handle(), 0,
                                 draw_commands.size(), 1);
    }
};

} // namespace graphics_pipeline::geometry
