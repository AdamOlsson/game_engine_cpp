#include "graphics_pipeline/text/TextRenderer2.h"
#include "graphics_pipeline/SwapDescriptorSetBuilder.h"

namespace graphics_pipeline::text {

TextRenderer2::TextRenderer2(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                             const RendererOpts &opts)
    : m_ctx(ctx) {

    graphics_pipeline::PipelineOpts pipeline_opts{};
    pipeline_opts.swap_chain.extent = opts.swap_chain.extent;
    pipeline_opts.swap_chain.render_pass = opts.swap_chain.render_pass;
    pipeline_opts.push_constant_range = opts.push_constant_range;
    pipeline_opts.descriptor.layout = TextRenderer2::get_descriptor_set_layout(ctx);
    m_text_pipeline = TextPipeline(ctx, pipeline_opts);

    const size_t max_frames_in_flight = 2;
    constexpr size_t max_text_instances = 16;
    constexpr size_t max_glyph_instances = 1024;

    m_text_format_instances = vulkan::buffers::StorageBuffer<TextFormatSBO2>(
        ctx, max_text_instances, max_frames_in_flight);
    m_glyph_instances = vulkan::buffers::StorageBuffer<TextGlyphSBO2>(
        ctx, max_glyph_instances, max_frames_in_flight);

    m_descriptor_pool = vulkan::DescriptorPool(
        ctx, vulkan::DescriptorPoolOpts{.max_num_descriptor_sets = 2,
                                        .num_storage_buffers = 2,
                                        .num_uniform_buffers = 0,
                                        .num_combined_image_samplers = 0});

    auto builder = SwapDescriptorSetBuilder(2);
    builder.add_storage_buffer(0, vulkan::DescriptorBufferInfo::from_vector(
                                      m_text_format_instances.get_reference()));
    builder.add_storage_buffer(
        1, vulkan::DescriptorBufferInfo::from_vector(m_glyph_instances.get_reference()));
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);
}

vulkan::DescriptorSetLayout TextRenderer2::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    builder.add_storage_buffer_binding(1, VK_SHADER_STAGE_VERTEX_BIT);
    return builder.build(ctx);
}

void TextRenderer2::write_to_format_buffer(
    const std::vector<TextFormatSBO2> &instance_data, const size_t offset) {
    m_text_format_instances.write(instance_data, offset);
}

std::vector<vulkan::DrawIndexedIndirectCommand>
TextRenderer2::write_to_glyph_buffer(const std::vector<TextGlyphSBO2> &instance_data,
                                     const size_t offset) {
    std::vector<vulkan::DrawIndexedIndirectCommand> draw_commands;

    // TODO: This information is available in m_glyph_draw_info;
    vulkan::DrawIndexedIndirectCommand draw_command;
    /*draw_command.firstIndex = 0;*/
    /*draw_command.firstInstance = 0;*/
    /*draw_command.indexCount = m_quad_index_buffer.num_indices;*/
    /*draw_command.instanceCount = instance_data.size();*/
    /*draw_command.vertexOffset = 0;*/

    DEBUG_ASSERT(false, "Not yet implemented.");
    return draw_commands;
}

bool TextRenderer2::is_font_loaded() { return m_font.is_loaded(); }

void TextRenderer2::load_font(vulkan::CommandBufferManager *command_buffer_manager,
                              const std::string &font_path) {

    m_font = font::Font(font_path);

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

} // namespace graphics_pipeline::text
