#pragma once

#include "graphics_pipeline/RendererOpts.h"
#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/Texture.h"
#include "graphics_pipeline/quad/QuadPipeline.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/Sampler.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/StagedGpuBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
namespace graphics_pipeline::quad {

struct QuadPipelineSBO;
class QuadSBOHandle;

class QuadRenderer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    QuadPipeline m_quad_pipeline;

    vulkan::buffers::VertexBuffer<vulkan::Vertex> m_quad_vertex_buffer;
    vulkan::buffers::IndexBuffer m_quad_index_buffer;

    vulkan::DescriptorPool m_descriptor_pool;
    Texture m_texture;
    vulkan::Sampler m_sampler;

    SwapDescriptorSet m_descriptor_sets;

    struct {
        size_t next_id;
        size_t dense_count;
        std::vector<size_t> sparse;
        std::vector<size_t> reverse;
        std::vector<size_t> available;
        vulkan::buffers::StagedStorageBuffer<QuadPipelineSBO> dense;
    } m_sparse_set;

    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();
    bool contains(size_t id) const;

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

  public:
    QuadRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                 vulkan::CommandBufferManager *command_buffer_manager,
                 RendererOpts &opts);

    QuadRenderer(QuadRenderer &&) noexcept = delete;
    QuadRenderer &operator=(QuadRenderer &&) noexcept = delete;

    QuadRenderer(const QuadRenderer &) = delete;
    QuadRenderer &operator=(const QuadRenderer &) = delete;

    QuadSBOHandle request_render_slot();
    void return_render_slot(graphics_pipeline::quad::QuadSBOHandle &handle);
    QuadPipelineSBO &get_instance(const graphics_pipeline::quad::QuadSBOHandle &handle);
    void sync_render_slots();

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer,
                PushConstantType *push_constant, const int num_instances) {

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_quad_pipeline.get_layout(),
                               m_quad_pipeline.get_push_constant_stage(), 0,
                               sizeof(*push_constant), push_constant);
        }

        // Handle descriptor set
        const vulkan::DescriptorSet set = m_descriptor_sets.get_current();
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_quad_pipeline.get_layout(), 0, 1, &set, 0, nullptr);

        m_quad_pipeline.bind(command_buffer);

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
