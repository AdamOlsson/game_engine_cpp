#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipeline.h"
#include "game_engine_sdk/render_engine/Geometry.h"
#include "game_engine_sdk/render_engine/resources/shaders/fragment/quad/quad.h"
#include "game_engine_sdk/render_engine/resources/shaders/vertex/quad/quad.h"

// TODO:
// 1. Implement the QuadPipeline and use it in map generation example

using namespace graphics_pipeline;

QuadPipeline::QuadPipeline(
    std::shared_ptr<graphics_context::GraphicsContext> ctx,
    CommandBufferManager *command_buffer_manager, SwapChainManager *swap_chain_manager,
    const std::optional<vulkan::DescriptorSetLayout> &descriptor_set_layout)
    : m_ctx(ctx), m_quad_vertex_buffer(VertexBuffer(m_ctx, Geometry::quad_vertices,
                                                    command_buffer_manager)),
      m_quad_index_buffer(
          IndexBuffer(m_ctx, Geometry::quad_indices, command_buffer_manager)),
      m_pipeline_layout(vulkan::PipelineLayout(ctx, descriptor_set_layout, {})),
      m_pipeline(
          vulkan::Pipeline(ctx, m_pipeline_layout,
                           vulkan::ShaderModule(ctx, *QuadVertex::create_resource()),
                           vulkan::ShaderModule(ctx, *QuadFragment::create_resource()),
                           *swap_chain_manager)) {}

void QuadPipeline::render(const vulkan::CommandBuffer &command_buffer,
                          std::optional<QuadPipelineDescriptorSet> &descriptor_set,
                          const int num_instances) {

    /*vkCmdPushConstants(command_buffer, m_pipeline_layout, stage_flags, 0,
     * sizeof(values),*/
    /*                   &values);*/

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    if (descriptor_set.has_value()) {
        const vulkan::DescriptorSet set = descriptor_set->get_next();
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipeline_layout, 0, 1, &set, 0, nullptr);
    }

    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_quad_vertex_buffer.buffer,
                           &vertex_buffers_offset);
    vkCmdBindIndexBuffer(command_buffer, m_quad_index_buffer.buffer, 0,
                         VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(command_buffer, m_quad_index_buffer.num_indices, num_instances, 0, 0,
                     0);
}
