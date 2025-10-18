#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipeline.h"
#include "game_engine_sdk/render_engine/Geometry.h"
#include "game_engine_sdk/render_engine/descriptors/DescriptorSetBuilder.h"

// TODO:
// 1. Implement the QuadPipeline and use it in map generation example
// 2. Implement the QuadPipeline::render() function
// 3. Pass the QuadPipelineDescriptorSet in the QuadPipeline::render() function
// 4. Refactor DescriptorSet class

using namespace graphics_pipeline;

QuadPipelineDescriptorSet::QuadPipelineDescriptorSet(
    std::shared_ptr<graphics_context::GraphicsContext> &ctx,
    DescriptorPool &descriptor_pool, QuadPipelineDescriptorSetOpts &&opts)
    : m_descriptor_set(
          DescriptorSetBuilder(opts.num_sets)
              .add_storage_buffer(0, std::move(opts.storage_buffer_refs))
              .add_uniform_buffer(1, std::move(opts.uniform_buffer_refs))
              .add_combined_image_sampler(2, opts.combined_image_sampler_infos)
              .build(ctx, descriptor_pool)) {}

const DescriptorSetLayout &QuadPipelineDescriptorSet::get_layout() {
    return m_descriptor_set.get_layout();
}

const DescriptorSet *QuadPipelineDescriptorSet::handle() const {
    return &m_descriptor_set;
}

QuadPipeline::QuadPipeline(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                           CommandBufferManager *command_buffer_manager)
    : m_ctx(ctx), m_quad_vertex_buffer(VertexBuffer(m_ctx, Geometry::quad_vertices,
                                                    command_buffer_manager)),
      m_quad_index_buffer(
          IndexBuffer(m_ctx, Geometry::quad_indices, command_buffer_manager)) {}

void QuadPipeline::render(const vulkan::CommandBuffer &command_buffer,
                          const QuadPipelineDescriptorSet &descriptor_set) {
    /*vkCmdPushConstants(command_buffer, m_pipeline_layout, stage_flags, 0,
     * sizeof(values),*/
    /*                   &values);*/

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    auto set = descriptor_set.handle();
    /*vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,*/
    /*                        m_pipeline_layout, 0, 1, &(*set), 0, nullptr);*/

    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_quad_vertex_buffer.buffer,
                           &vertex_buffers_offset);
    vkCmdBindIndexBuffer(command_buffer, m_quad_index_buffer.buffer, 0,
                         VK_INDEX_TYPE_UINT16);
    /*vkCmdDrawIndexed(command_buffer, index_buffer.num_indices, num_instances, 0, 0,
     * 0);*/
}
