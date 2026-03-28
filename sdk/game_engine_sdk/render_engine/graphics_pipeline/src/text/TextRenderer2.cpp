#include "graphics_pipeline/text/TextRenderer2.h"
#include "font/Polygon.h"
#include "graphics_pipeline/SwapDescriptorSetBuilder.h"
#include "graphics_pipeline/text/TextFormatter.h"
#include "math/winding.h"
#include "triangulation/mapbox/earcut.h"

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

bool TextRenderer2::is_font_loaded() { return m_font_loader.has_value(); }

void TextRenderer2::load_font(vulkan::CommandBufferManager *command_buffer_manager,
                              font::FontLoader &&font_loader) {

    if (is_font_loaded()) {
        throw std::runtime_error("Error: Font is already loaded.");
    }

    m_glyph_draw_info.reserve(font_loader.get_num_glyphs());

    std::vector<GlyphVertex> vertices;
    std::vector<uint16_t> indices;

    for (const auto glyph_index : font_loader) {
        font::GlyphOutlines outlines = font_loader.get_glyph_outline(glyph_index);
        const font::FontFill exterior_fill = outlines.fill;

        const size_t first_index = indices.size();
        if (!outlines.line_segments.empty()) {
            const std::vector<font::Polygon> glyph_polygons =
                font::Polygon::construct_polygons(outlines);

            for (const font::Polygon polygon : glyph_polygons) {
                const size_t first_vertex = vertices.size();

                const std::vector<unsigned int> triangle_indices =
                    mapbox::earcut(polygon.get_outlines());

                // First load write all vertices from earcut to the vertex buffer
                for (const std::vector<std::pair<float, float>> &outline :
                     polygon.get_outlines()) {
                    for (const std::pair<float, float> &vertex : outline) {
                        vertices.emplace_back(vertex.first, vertex.second, 0.0f, 0.0f,
                                              0.0f, 0.0f);
                    }
                }

                // Secondly write the indices forming the triangles from earcut into
                // to index buffer
                for (const unsigned int index : triangle_indices) {
                    indices.emplace_back(first_vertex + index);
                }

                // Thirdly write all curve segments into vertex and index buffer
                // (which by nature of bezier curves are already triangulated)
                const auto quad_curves = polygon.get_quadratic_curves();
                for (size_t outline_index = 0; outline_index < quad_curves.size();
                     outline_index++) {

                    const std::vector<std::array<std::pair<float, float>, 3>> &outline =
                        quad_curves[outline_index];

                    for (const std::array<std::pair<float, float>, 3> &curve : outline) {
                        const bool ccw = math::is_counter_clockwise_winding(curve);

                        const bool wants_right_fill =
                            (exterior_fill == font::FontFill::Right);
                        const float winding_order =
                            (wants_right_fill == !ccw) ? 1.0f : -1.0f;

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[0].first, curve[0].second,
                                              winding_order, 0.0f, 0.0f, 1.0f);

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[1].first, curve[1].second,
                                              winding_order, 0.5f, 0.0f, 1.0f);

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[2].first, curve[2].second,
                                              winding_order, 1.0f, 1.0f, 1.0f);
                    }
                }
            }
        }

        const size_t count = indices.size() - first_index;
        m_glyph_draw_info.emplace_back(count, first_index);
    }

    m_glyph_vertex_buffer =
        vulkan::buffers::VertexBuffer(m_ctx, vertices, command_buffer_manager);
    m_glyph_index_buffer =
        vulkan::buffers::IndexBuffer(m_ctx, indices, command_buffer_manager);
    m_font_loader = std::move(font_loader);
    m_formatter = TextFormatter(&m_font_loader.value());
}

} // namespace graphics_pipeline::text
