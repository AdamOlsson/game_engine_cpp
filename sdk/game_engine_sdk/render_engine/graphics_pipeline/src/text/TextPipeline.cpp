#include "graphics_pipeline/text/TextPipeline.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "shaders/text_fragment_shader.h"
#include "shaders/text_vertex_shader.h"
#include "vulkan/vulkan_core.h"

graphics_pipeline::text::TextPipeline::TextPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    vulkan::SwapChainManager *swap_chain_manager,
    const vulkan::DescriptorSetLayout *descriptor_set_layout,
    const vulkan::PushConstantRange *push_constant_range)
    : m_ctx(ctx),
      m_push_constant_stage(push_constant_range ? push_constant_range->stageFlags : 0),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, descriptor_set_layout, push_constant_range)) {

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
