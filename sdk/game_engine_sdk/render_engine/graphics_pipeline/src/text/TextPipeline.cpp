#include "graphics_pipeline/text/TextPipeline.h"
#include "shaders/text_fragment_shader.h"
#include "shaders/text_vertex_shader.h"
#include "triangulation/earcut.h"
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
        *swap_chain_manager, vulkan::PipelineOpts{});
}

void graphics_pipeline::text::TextPipeline::load_font(
    vulkan::CommandBufferManager *command_buffer_manager, const font::OTFFont &font) {

    std::vector<vulkan::DrawIndexedIndirectCommand> index_draw_commands;
    size_t instance_offset_count = 0;
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    for (const auto &glyph : font.glyphs) {

        // Note: When I allow of composite glyphs, the cmap will not longer be valid
        // as each additional outline of one glyph offsets all following vertices one
        // index
        const auto &outline = glyph.outlines[0];

        const size_t first_vertex_idx = vertices.size();
        for (const auto &point : outline) {
            vertices.emplace_back(static_cast<float>(point.first),
                                  -1.0f * static_cast<float>(point.second), 0.0f);
        }

        const std::vector<std::array<size_t, 3>> triangles =
            triangulation::earclip(outline);

        const size_t first_index_idx = indices.size();
        for (const auto &triangle : triangles) {
            indices.push_back(triangle[0] + first_vertex_idx);
            indices.push_back(triangle[1] + first_vertex_idx);
            indices.push_back(triangle[2] + first_vertex_idx);
        }

        index_draw_commands.push_back(vulkan::DrawIndexedIndirectCommand{
            .indexCount = static_cast<uint32_t>(indices.size() - first_index_idx),
            .instanceCount = 1,
            .firstIndex = static_cast<uint32_t>(first_index_idx),
            .firstInstance = static_cast<uint32_t>(instance_offset_count),
        });

        // TODO: Handle multiple instances of a glyph
        instance_offset_count++;
    }

    m_glyph_vertex_buffer =
        vulkan::buffers::VertexBuffer(m_ctx, vertices, command_buffer_manager);
    m_glyph_index_buffer =
        vulkan::buffers::IndexBuffer(m_ctx, indices, command_buffer_manager);

    m_draw_command_buffer =
        vulkan::buffers::StorageBuffer<vulkan::DrawIndexedIndirectCommand>(
            m_ctx, index_draw_commands.size(), VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT);

    for (auto &command : index_draw_commands) {
        m_draw_command_buffer->push_back(std::move(command));
    }

    m_draw_command_buffer->transfer();
    m_font = font;
}
