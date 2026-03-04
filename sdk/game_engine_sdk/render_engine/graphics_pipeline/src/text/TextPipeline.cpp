#include "graphics_pipeline/text/TextPipeline.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "shaders/text_fragment_shader.h"
#include "shaders/text_vertex_shader.h"
#include "vulkan/vulkan_core.h"

graphics_pipeline::text::TextPipeline::TextPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    graphics_pipeline::PipelineOpts &opts)
    : m_ctx(ctx), m_push_constant(opts.push_constant_range),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, &opts.descriptor.layout, &m_push_constant)) {

    auto quad_vert = TextVertexShader::create_resource();
    auto quad_frag = TextFragmentShader::create_resource();

    VkVertexInputBindingDescription binding_description =
        GlyphVertex::get_binding_description();
    std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions =
        GlyphVertex::get_attribute_descriptions();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout,
        vulkan::ShaderModule(ctx, quad_vert->bytes(), quad_vert->length()),
        vulkan::ShaderModule(ctx, quad_frag->bytes(), quad_frag->length()),
        vulkan::PipelineOpts{
            .vertex_input_info =
                {
                    .binding_description = binding_description,
                    .attribute_descriptions_count = attribute_descriptions.size(),
                    .attribute_descriptions = attribute_descriptions.data(),
                },
            .rasterizer =
                {
                    .cull_mode = VK_CULL_MODE_NONE,
                },
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
