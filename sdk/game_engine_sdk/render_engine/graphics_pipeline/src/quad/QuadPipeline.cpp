#include "graphics_pipeline/quad/QuadPipeline.h"
#include "shaders/quad_fragment_shader.h"
#include "shaders/quad_vertex_shader.h"

graphics_pipeline::quad::QuadPipeline::QuadPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    graphics_pipeline::PipelineOpts &opts)
    : m_ctx(ctx), m_push_constant(opts.push_constant_range),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, &opts.descriptor.layout, &m_push_constant)) {

    auto quad_vert = QuadVertexShader::create_resource();
    auto quad_frag = QuadFragmentShader::create_resource();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout,
        vulkan::ShaderModule(ctx, quad_vert->bytes(), quad_vert->length()),
        vulkan::ShaderModule(ctx, quad_frag->bytes(), quad_frag->length()),
        vulkan::PipelineOpts{
            .viewport =
                {
                    .width = static_cast<float>(opts.swap_chain.extent.width),
                    .height = static_cast<float>(opts.swap_chain.extent.height),
                },
            .scissor =
                {
                    .extent = opts.swap_chain.extent,
                },

            .pipeline_info =
                {
                    .render_pass = *opts.swap_chain.render_pass,
                },
        });
}
