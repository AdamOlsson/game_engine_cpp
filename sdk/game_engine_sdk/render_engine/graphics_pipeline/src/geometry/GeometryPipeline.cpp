#include "graphics_pipeline/geometry/GeometryPipeline.h"
#include "shaders/geometry_fragment_shader.h"
#include "shaders/geometry_vertex_shader.h"

graphics_pipeline::geometry::GeometryPipeline::GeometryPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx, const PipelineOpts &opts)
    : m_ctx(ctx), m_push_constant(opts.push_constant_range),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, &opts.descriptor.layout, &m_push_constant)) {

    auto vert = GeometryVertexShader::create_resource();
    auto frag = GeometryFragmentShader::create_resource();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout, vulkan::ShaderModule(ctx, vert->bytes(), vert->length()),
        vulkan::ShaderModule(ctx, frag->bytes(), frag->length()),
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
