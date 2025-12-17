#include "graphics_pipeline/text/TextPipeline.h"
#include "shaders/text_fragment_shader.h"
#include "shaders/text_vertex_shader.h"
#include "vulkan/vulkan_core.h"

graphics_pipeline::text::TextPipeline::TextPipeline(
    std::shared_ptr<vulkan::context::GraphicsContext> ctx,
    vulkan::CommandBufferManager *command_buffer_manager,
    vulkan::SwapChainManager *swap_chain_manager,
    const vulkan::DescriptorSetLayout *descriptor_set_layout,
    const vulkan::PushConstantRange *push_constant_range)
    : m_ctx(ctx), m_glyph_vertex_buffer(std::nullopt), m_glyph_index_buffer(std::nullopt),
      m_push_constant_stage(push_constant_range ? push_constant_range->stageFlags : 0),
      m_pipeline_layout(
          vulkan::PipelineLayout(ctx, descriptor_set_layout, push_constant_range)) {

    auto quad_vert = TextVertexShader::create_resource();
    auto quad_frag = TextFragmentShader::create_resource();

    m_pipeline = vulkan::Pipeline(
        ctx, m_pipeline_layout,
        vulkan::ShaderModule(ctx, quad_vert->bytes(), quad_vert->length()),
        vulkan::ShaderModule(ctx, quad_frag->bytes(), quad_frag->length()),
        *swap_chain_manager,
        vulkan::PipelineOpts{
            .assembler = {.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST},
        });
}

void graphics_pipeline::text::TextPipeline::load_font(
    vulkan::CommandBufferManager *command_buffer_manager, const font::OTFFont &font) {

    std::vector<Vertex> vertices;
    for (const auto &glyph : font.m_font_table_cff.glyphs) {
        for (const auto &point : glyph.outlines) {
            vertices.emplace_back(static_cast<float>(point.first),
                                  -1.0f * static_cast<float>(point.second), 0.0f);
        }
    }

    m_glyph_vertex_buffer =
        vulkan::buffers::VertexBuffer(m_ctx, vertices, command_buffer_manager);
}
