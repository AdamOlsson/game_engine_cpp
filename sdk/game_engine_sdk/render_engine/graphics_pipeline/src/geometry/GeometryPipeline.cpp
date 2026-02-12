#include "graphics_pipeline/geometry/GeometryPipeline.h"
#include "shaders/geometry_fragment_shader.h"
#include "shaders/geometry_vertex_shader.h"

graphics_pipeline::geometry::GeometryPipeline::GeometryPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    vulkan::SwapChainManager *swap_chain_manager,
    const vulkan::DescriptorSetLayout *descriptor_set_layout,
    const vulkan::PushConstantRange *push_constant_range)
    : m_ctx(ctx),
      m_push_constant_stage(push_constant_range ? push_constant_range->stageFlags : 0),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, descriptor_set_layout, push_constant_range)) {

    auto vert = GeometryVertexShader::create_resource();
    auto frag = GeometryFragmentShader::create_resource();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout, vulkan::ShaderModule(ctx, vert->bytes(), vert->length()),
        vulkan::ShaderModule(ctx, frag->bytes(), frag->length()),
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
