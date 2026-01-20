#include "graphics_pipeline/quad/QuadPipeline.h"
#include "shaders/quad_fragment_shader.h"
#include "shaders/quad_vertex_shader.h"

namespace {
const std::vector<uint16_t> m_quad_indices = {0, 1, 2, 0, 2, 3};
const std::vector<vulkan::Vertex> m_quad_vertices = {
    vulkan::Vertex(-0.5f, -0.5f, 0.0f),
    vulkan::Vertex(-0.5f, 0.5f, 0.0f),
    vulkan::Vertex(0.5f, 0.5f, 0.0f),
    vulkan::Vertex(0.5f, -0.5f, 0.0f),
};
} // namespace

graphics_pipeline::quad::QuadPipeline::QuadPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    vulkan::SwapChainManager *swap_chain_manager,
    const vulkan::DescriptorSetLayout *descriptor_set_layout,
    const vulkan::PushConstantRange *push_constant_range)
    : m_ctx(ctx), m_quad_vertex_buffer(vulkan::buffers::VertexBuffer(
                      m_ctx, m_quad_vertices, command_buffer_manager)),
      m_quad_index_buffer(
          vulkan::buffers::IndexBuffer(m_ctx, m_quad_indices, command_buffer_manager)),
      m_push_constant_stage(push_constant_range ? push_constant_range->stageFlags : 0),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, descriptor_set_layout, push_constant_range)) {

    auto quad_vert = QuadVertexShader::create_resource();
    auto quad_frag = QuadFragmentShader::create_resource();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout,
        vulkan::ShaderModule(ctx, quad_vert->bytes(), quad_vert->length()),
        vulkan::ShaderModule(ctx, quad_frag->bytes(), quad_frag->length()),
        vulkan::PipelineOpts{
            .viewport =
                {
                    .width = static_cast<float>(
                        swap_chain_manager->m_swap_chain.m_extent.width),
                    .height = static_cast<float>(
                        swap_chain_manager->m_swap_chain.m_extent.height),
                },
            .scissor =
                {
                    .extent = swap_chain_manager->m_swap_chain.m_extent,
                },

            .pipeline_info =
                {
                    .render_pass = swap_chain_manager->m_swap_chain.m_render_pass,
                },
        });
}
