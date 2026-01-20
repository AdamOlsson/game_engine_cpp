#include "graphics_pipeline/text/TextRenderer.h"
#include "graphics_pipeline/DescriptorSetLayoutBuilder.h"
#include "graphics_pipeline/Polygon.h"
#include "graphics_pipeline/SwapDescriptorSetBuilder.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "graphics_pipeline/text/TextPipeline.h"
#include "math/Matrix.h"
#include "math/winding.h"
#include "triangulation/mapbox/earcut.h"
#include "vulkan/CommandBufferManager.h"

graphics_pipeline::text::TextRenderer::TextRenderer(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
    vulkan::SwapChainManager *swap_chain_manager,
    const VkPushConstantRange *push_constant_range)
    : m_ctx(ctx) {

    const auto layout = TextRenderer::get_descriptor_set_layout(ctx);
    m_text_pipeline = TextPipeline(ctx, swap_chain_manager, &layout, push_constant_range);

    m_text_instances = vulkan::buffers::SwapStorageBuffer<TextSBO>(ctx, 2, 16);
    m_glyph_instances = vulkan::buffers::SwapStorageBuffer<GlyphSBO>(ctx, 2, 1024);

    m_descriptor_pool = vulkan::DescriptorPool(
        ctx, vulkan::DescriptorPoolOpts{.max_num_descriptor_sets = 2,
                                        .num_storage_buffers = 2,
                                        .num_uniform_buffers = 0,
                                        .num_combined_image_samplers = 0});
    auto builder = SwapDescriptorSetBuilder(2);
    builder.add_storage_buffer(0, vulkan::DescriptorBufferInfo::from_vector(
                                      m_text_instances.get_buffer_references()));
    builder.add_storage_buffer(1, vulkan::DescriptorBufferInfo::from_vector(
                                      m_glyph_instances.get_buffer_references()));
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);
}

vulkan::DescriptorSetLayout
graphics_pipeline::text::TextRenderer::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    builder.add_storage_buffer_binding(1, VK_SHADER_STAGE_VERTEX_BIT);
    return builder.build(ctx);
}

void graphics_pipeline::text::TextRenderer::load_font(
    vulkan::CommandBufferManager *command_buffer_manager,
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
            const std::vector<graphics_pipeline::Polygon> glyph_polygons =
                graphics_pipeline::Polygon::construct_polygons(outlines);

            for (const graphics_pipeline::Polygon polygon : glyph_polygons) {
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

                // Secondly write the indices forming the triangles from earcut into to
                // index buffer
                for (const unsigned int index : triangle_indices) {
                    indices.emplace_back(first_vertex + index);
                }

                // Thirdly write all curve segments into vertex and index buffer (which by
                // nature of bezier curves are already triangulated)
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
}

graphics_pipeline::text::TextString
graphics_pipeline::text::TextRenderer::create_text(const font::Unicode &codepoint) {

    if (!is_font_loaded()) {
        throw std::runtime_error(
            "Error: can't create text because a font is not loaded.");
    }

    unsigned int text_id = m_text_count;
    unsigned int first_glyph = m_glyph_count;

    m_text_instances.push_back(TextSBO{.model_matrix = glm::mat4(1.0f)});

    std::vector<uint32_t> index_count;
    std::vector<uint32_t> first_index;

    index_count.reserve(codepoint.size());
    first_index.reserve(codepoint.size());

    const font::FontBBox bbox = m_font_loader->get_font_bbox();
    const float column_width = bbox.x_max - bbox.x_min + 150.0f;
    for (size_t i = 0; i < codepoint.size(); i++) {
        const auto &c = codepoint[i];
        m_glyph_instances.push_back(
            GlyphSBO{.text_id = static_cast<uint16_t>(text_id),
                     .offset = math::Matrix().translate(column_width * i, 0.0f, 0.0f)});

        std::pair<size_t, size_t> draw_info =
            m_glyph_draw_info[m_font_loader->get_glyph_index(c)];
        index_count.push_back(draw_info.first);
        first_index.push_back(draw_info.second);

        m_glyph_count++;
    }
    m_text_count++;

    m_text_instances.transfer();
    m_glyph_instances.transfer();

    return TextString{
        .first_glyph = first_glyph,
        .glyph_count = static_cast<uint32_t>(codepoint.size()),
        .index_count = std::move(index_count),
        .first_index = std::move(first_index),
    };
}

graphics_pipeline::text::TextString
graphics_pipeline::text::TextRenderer::get_font_showcase_text() {
    if (!is_font_loaded()) {
        throw std::runtime_error(
            "Error: Can't render font showcase because font is not loaded.");
    }

    unsigned int text_id = m_text_count;
    unsigned int first_glyph = m_glyph_count;

    m_text_instances.push_back(TextSBO{.model_matrix = glm::mat4(1.0f)});

    const size_t num_glyphs = m_font_loader->get_num_glyphs();

    std::vector<uint32_t> index_count;
    std::vector<uint32_t> first_index;

    index_count.reserve(num_glyphs);
    first_index.reserve(num_glyphs);

    const font::FontBBox bbox = m_font_loader->get_font_bbox();
    const float column_width = bbox.x_max - bbox.x_min;
    const float column_height = bbox.x_max - bbox.x_min;

    const float scale = 0.01f;
    const size_t num_cols = 8;
    for (size_t gid = 0; gid < num_glyphs; gid++) {

        const float x = column_width * (gid % num_cols);
        const float y = column_height * (gid / num_cols);

        m_glyph_instances.push_back(GlyphSBO{
            .text_id = static_cast<uint16_t>(text_id),
            .offset = math::Matrix().translate(x, y, 0.0f).scale(0.1),
        });

        const std::pair<size_t, size_t> glyph_info = m_glyph_draw_info[gid];
        index_count.push_back(glyph_info.first);
        first_index.push_back(glyph_info.second);
        m_glyph_count++;
    }

    m_text_count++;

    m_text_instances.transfer();
    m_glyph_instances.transfer();

    return TextString{
        .first_glyph = first_glyph,
        .glyph_count = static_cast<uint32_t>(num_glyphs),
        .index_count = std::move(index_count),
        .first_index = std::move(first_index),
    };
}

void graphics_pipeline::text::TextRenderer::_render(
    const vulkan::CommandBuffer &command_buffer, const TextString &text) {
    for (size_t i = 0; i < text.glyph_count; i++) {
        vkCmdDrawIndexed(command_buffer, text.index_count[i], 1, text.first_index[i], 0,
                         text.first_glyph + i);
    }
}
