#include "graphics_pipeline/quad/QuadRenderer.h"
#include "graphics_pipeline/DescriptorSetLayoutBuilder.h"

namespace {
const std::vector<uint16_t> m_quad_indices = {0, 1, 2, 0, 2, 3};
const std::vector<vulkan::Vertex> m_quad_vertices = {
    vulkan::Vertex(-0.5f, -0.5f, 0.0f),
    vulkan::Vertex(-0.5f, 0.5f, 0.0f),
    vulkan::Vertex(0.5f, 0.5f, 0.0f),
    vulkan::Vertex(0.5f, -0.5f, 0.0f),
};
} // namespace

graphics_pipeline::quad::QuadRenderer::QuadRenderer(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    vulkan::SwapChainManager *swap_chain_manager,
    const VkPushConstantRange *push_constant_range,
    const vulkan::DescriptorSetLayout &layout)
    : m_ctx(ctx), m_quad_vertex_buffer(vulkan::buffers::VertexBuffer(
                      m_ctx, m_quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(
          vulkan::buffers::IndexBuffer(m_ctx, m_quad_indices, command_buffer_manager)) {

    /*const auto layout = QuadRenderer::get_descriptor_set_layout(ctx);*/
    m_quad_pipeline = QuadPipeline(m_ctx, command_buffer_manager, swap_chain_manager,
                                   &layout, push_constant_range);

    /*m_descriptor_pool = vulkan::DescriptorPool(*/
    /*    ctx, vulkan::DescriptorPoolOpts{.max_num_descriptor_sets = 2,*/
    /*                                    .num_storage_buffers = 2,*/
    /*                                    .num_uniform_buffers = 0,*/
    /*                                    .num_combined_image_samplers = 0});*/

    /*auto builder = SwapDescriptorSetBuilder(2);*/
    /*builder.add_storage_buffer(*/
    /*    0, vulkan::DescriptorBufferInfo::from_vector(m_instances.get_reference()));*/
    /*m_descriptor_sets = builder.build(ctx, m_descriptor_pool);*/
}

vulkan::DescriptorSetLayout
graphics_pipeline::quad::QuadRenderer::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    builder.add_combined_image_sampler_binding(2, VK_SHADER_STAGE_FRAGMENT_BIT);
    return builder.build(ctx);
}
