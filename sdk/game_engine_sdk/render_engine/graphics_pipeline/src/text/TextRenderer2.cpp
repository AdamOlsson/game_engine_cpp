#include "graphics_pipeline/text/TextRenderer2.h"
#include "graphics_pipeline/SwapDescriptorSetBuilder.h"

namespace graphics_pipeline::text {

const size_t max_frames_in_flight = 2;
constexpr size_t max_text_instances = 16;
constexpr size_t max_glyph_instances = 1024;
constexpr size_t max_draw_commands = 1024;

TextRenderer2::TextRenderer2(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                             const RendererOpts &opts)
    : m_ctx(ctx), m_text_format_instances(vulkan::buffers::StorageBuffer<TextFormatSBO2>(
                      ctx, max_text_instances, max_frames_in_flight)),
      m_glyph_instances(vulkan::buffers::StorageBuffer<TextGlyphSBO2>(
          ctx, max_glyph_instances, max_frames_in_flight)),
      m_draw_commands(vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand>(
          m_ctx, max_draw_commands, max_frames_in_flight)) {

    m_descriptor_pool = vulkan::DescriptorPool(ctx, opts.text.pool_opts);

    auto builder = SwapDescriptorSetBuilder(2);
    builder.add_storage_buffer(0, vulkan::DescriptorBufferInfo::from_vector(
                                      m_text_format_instances.get_reference()));
    builder.add_storage_buffer(
        1, vulkan::DescriptorBufferInfo::from_vector(m_glyph_instances.get_reference()));
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);

    graphics_pipeline::PipelineOpts pipeline_opts{};
    pipeline_opts.swap_chain.extent = opts.swap_chain.extent;
    pipeline_opts.swap_chain.render_pass = opts.swap_chain.render_pass;
    pipeline_opts.push_constant_range = opts.push_constant_range;
    pipeline_opts.descriptor.layout = TextRenderer2::get_descriptor_set_layout(ctx);
    m_text_pipeline = TextPipeline(ctx, pipeline_opts);
}

vulkan::DescriptorSetLayout TextRenderer2::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    builder.add_storage_buffer_binding(1, VK_SHADER_STAGE_VERTEX_BIT);
    return builder.build(ctx);
}

void TextRenderer2::write_to_format_buffer(const std::vector<font::TextFormat> &format,
                                           const size_t offset) {

    std::vector<TextFormatSBO2> instance_data;
    instance_data.reserve(format.size());
    for (const auto &f : format) {
        instance_data.emplace_back(f.position, f.font_color, f.font_size);
    }
    m_text_format_instances.write(instance_data, offset);
}

std::vector<vulkan::DrawIndexedIndirectCommand>
TextRenderer2::write_to_glyph_buffer(const font::Text &text, const size_t text_format_id,
                                     const size_t offset) {

    // TODO: Improve TextRenderer2 draw call by instancing on each character instead of
    // simply creating one draw command per character. The result would become one draw
    // commands for every 'a' for example. This also requires the characters to be
    // sequentially in the instance buffer, even though they belon to different texts. As
    // an example: a a a a a a |  b b b b b b  | c c c
    std::vector<vulkan::DrawIndexedIndirectCommand> draw_commands;
    draw_commands.reserve(text.glyphs.size());

    std::vector<TextGlyphSBO2> glyph_instances;
    glyph_instances.reserve(text.glyphs.size());

    for (size_t i = 0; i < text.glyphs.size(); i++) {
        const auto &glyph = text.glyphs[i];

        TextGlyphSBO2 glyph_instance;

        glyph_instance.offset = math::Vector3(glyph.offset, 0.0f);
        glyph_instance.text_id = text_format_id;
        glyph_instances.push_back(glyph_instance);

        vulkan::DrawIndexedIndirectCommand draw_command;
        draw_command.firstIndex = glyph.first_index;
        draw_command.firstInstance = i + offset;
        draw_command.indexCount = glyph.index_count;
        draw_command.instanceCount = 1;
        draw_command.vertexOffset = 0;
        draw_commands.push_back(draw_command);
    }

    m_glyph_instances.write(glyph_instances, offset);

    return draw_commands;
}

void TextRenderer2::load_font(vulkan::CommandBufferManager *command_buffer_manager,
                              const std::string &font_filepath) {
    m_font = font::Font(font_filepath);

    std::vector<GlyphVertex> vertices;
    vertices.reserve(m_font.vertices.size());

    for (const auto &v : m_font.vertices) {
        vertices.emplace_back(v.x, v.y, v.z, v.u, v.v, v.w);
    }

    m_glyph_vertex_buffer =
        vulkan::buffers::VertexBuffer(m_ctx, vertices, command_buffer_manager);
    m_glyph_index_buffer =
        vulkan::buffers::IndexBuffer(m_ctx, m_font.indices, command_buffer_manager);
}

/*font::TextFormat TextRenderer2::create_text_format(const font::TextOpts &opts) {*/
/*    return m_font.create_text_format(opts);*/
/*}*/

/*font::Text TextRenderer2::create_text(const std::string &text,*/
/*                                      const font::TextOpts &opts) {*/
/*    return m_font.create_text(text, opts);*/
/*}*/

} // namespace graphics_pipeline::text
