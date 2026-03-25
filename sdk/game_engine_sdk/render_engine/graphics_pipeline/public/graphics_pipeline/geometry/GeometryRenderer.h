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
#include <limits>
#include <memory>

namespace graphics_pipeline::geometry {

class GeometrySBOHandle;

class GeometryRenderer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    vulkan::buffers::VertexBuffer<vulkan::Vertex> m_quad_vertex_buffer;
    vulkan::buffers::IndexBuffer m_quad_index_buffer;

    std::unique_ptr<GeometryPipeline> m_geometry_pipeline;

    vulkan::DescriptorPool m_descriptor_pool;

    SwapDescriptorSet m_descriptor_sets;

    struct {
        size_t next_id;
        size_t dense_count;
        std::vector<size_t> sparse;
        std::vector<size_t> reverse;
        std::vector<size_t> available;
        vulkan::buffers::StagedStorageBuffer<GeometryPipelineSBO> dense;
    } m_sparse_set;
    vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand> m_draw_commands;

    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();
    bool contains(size_t id) const;

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

  public:
    GeometryRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                     vulkan::CommandBufferManager *command_buffer_manager,
                     RendererOpts &opts);

    GeometryRenderer(GeometryRenderer &&) noexcept = delete;
    GeometryRenderer &operator=(GeometryRenderer &&) noexcept = delete;

    GeometryRenderer(const GeometryRenderer &) = delete;
    GeometryRenderer &operator=(const GeometryRenderer &) = delete;

    GeometrySBOHandle request_render_slot();
    void return_render_slot(GeometrySBOHandle &handle);
    GeometryPipelineSBO &get_instance(const GeometrySBOHandle &handle);
    void sync_render_slots();

    vulkan::DrawIndexedIndirectCommand
    write_instance_buffer(const std::vector<GeometryPipelineSBO> &instance_data,
                          const std::vector<size_t> &indices, const size_t offset = 0) {

        vulkan::DrawIndexedIndirectCommand draw_command;
        draw_command.firstIndex = 0;
        draw_command.firstInstance = 0;
        draw_command.indexCount = m_quad_index_buffer.num_indices;
        draw_command.instanceCount = indices.size();
        draw_command.vertexOffset = 0;

        m_sparse_set.dense.write_indices(instance_data, indices, offset);
        return draw_command;
    }

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer,
                PushConstantType *push_constant, const int num_instances) {

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
        vkCmdDrawIndexed(command_buffer, m_quad_index_buffer.num_indices, num_instances,
                         0, 0, 0);
    }

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
