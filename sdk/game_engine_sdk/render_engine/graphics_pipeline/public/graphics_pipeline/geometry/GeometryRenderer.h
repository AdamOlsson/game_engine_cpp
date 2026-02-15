#pragma once

#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/geometry/GeometryPipeline.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/SwapChainManager.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include <limits>
#include <memory>

namespace graphics_pipeline::geometry {

class GeometrySBOHandle;

struct GeometryRendererOpts {
    vulkan::DescriptorPoolOpts pool_opts = vulkan::DescriptorPoolOpts{
        .max_num_descriptor_sets = 2,
        .num_storage_buffers = 1,
        .num_uniform_buffers = 0,
        .num_combined_image_samplers = 0,
    };

    struct {
        size_t size = 256;
    } instance_buffer_opts;
};

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
        vulkan::buffers::StorageBuffer<GeometryPipelineSBO> dense;
    } m_sparse_set;

    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();
    bool contains(size_t id) const;

  public:
    GeometryRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                     vulkan::CommandBufferManager *command_buffer_manager,
                     vulkan::SwapChainManager *swap_chain_manager,
                     const vulkan::PushConstantRange *push_constant_range,
                     GeometryRendererOpts &&opts);

    GeometryRenderer(GeometryRenderer &&) noexcept = delete;
    GeometryRenderer &operator=(GeometryRenderer &&) noexcept = delete;

    GeometryRenderer(const GeometryRenderer &) = delete;
    GeometryRenderer &operator=(const GeometryRenderer &) = delete;

    GeometrySBOHandle request_render_slot();
    void return_render_slot(GeometrySBOHandle &handle);
    GeometryPipelineSBO &get_instance(const GeometrySBOHandle &handle);
    void sync_render_slots();

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
};

} // namespace graphics_pipeline::geometry
