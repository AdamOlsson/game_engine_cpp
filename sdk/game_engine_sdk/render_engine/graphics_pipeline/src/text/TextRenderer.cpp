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

    const size_t max_frames_in_flight = 2;
    constexpr size_t max_text_instances = 16;
    constexpr size_t max_glyph_instances = 1024;

    m_format_sparse_set.dense = vulkan::buffers::StorageBuffer<TextFormatSBO>(
        ctx, max_text_instances, max_frames_in_flight);
    m_glyph_sparse_set.dense = vulkan::buffers::StorageBuffer<TextGlyphSBO>(
        ctx, max_glyph_instances, max_frames_in_flight);

    m_descriptor_pool = vulkan::DescriptorPool(
        ctx, vulkan::DescriptorPoolOpts{.max_num_descriptor_sets = 2,
                                        .num_storage_buffers = 2,
                                        .num_uniform_buffers = 0,
                                        .num_combined_image_samplers = 0});
    auto builder = SwapDescriptorSetBuilder(2);
    builder.add_storage_buffer(0, vulkan::DescriptorBufferInfo::from_vector(
                                      m_format_sparse_set.dense.get_reference()));
    builder.add_storage_buffer(1, vulkan::DescriptorBufferInfo::from_vector(
                                      m_glyph_sparse_set.dense.get_reference()));
    m_descriptor_sets = builder.build(ctx, m_descriptor_pool);

    m_format_sparse_set.next_id = 0;
    m_format_sparse_set.dense_count = 0;
    m_format_sparse_set.sparse.resize(max_text_instances, INVALID_INDEX);
    m_format_sparse_set.reverse.reserve(max_text_instances);
    m_format_sparse_set.available.reserve(max_text_instances);
    m_format_sparse_set.dense.resize(max_text_instances);

    m_glyph_sparse_set.next_id = 0;
    m_glyph_sparse_set.dense_count = 0;
    m_glyph_sparse_set.sparse.resize(max_glyph_instances, INVALID_INDEX);
    m_glyph_sparse_set.reverse.reserve(max_glyph_instances);
    m_glyph_sparse_set.available.reserve(max_glyph_instances);
    m_glyph_sparse_set.dense.resize(max_glyph_instances);
}

vulkan::DescriptorSetLayout
graphics_pipeline::text::TextRenderer::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    builder.add_storage_buffer_binding(1, VK_SHADER_STAGE_VERTEX_BIT);
    return builder.build(ctx);
}

bool graphics_pipeline::text::TextRenderer::contains_format(size_t id) const {
    return id < m_format_sparse_set.sparse.size() &&
           m_format_sparse_set.sparse[id] != INVALID_INDEX;
}

bool graphics_pipeline::text::TextRenderer::contains_glyph(size_t id) const {
    return id < m_glyph_sparse_set.sparse.size() &&
           m_glyph_sparse_set.sparse[id] != INVALID_INDEX;
}

graphics_pipeline::text::TextFormatSBO &
graphics_pipeline::text::TextRenderer::get_text_format_instance(
    const graphics_pipeline::text::TextFormatSBOHandle &handle) {
    return m_format_sparse_set.dense[m_format_sparse_set.sparse[handle.id]];
}

graphics_pipeline::text::TextGlyphSBO &
graphics_pipeline::text::TextRenderer::get_text_glyph_instance(
    const graphics_pipeline::text::TextGlyphSBOHandle &handle) {
    return m_glyph_sparse_set.dense[m_glyph_sparse_set.sparse[handle.id]];
}

graphics_pipeline::text::TextFormatSBOHandle
graphics_pipeline::text::TextRenderer::request_format_slot() {

    size_t format_id = m_format_sparse_set.available.empty()
                           ? m_format_sparse_set.next_id++
                           : m_format_sparse_set.available.back();

    DEBUG_ASSERT(format_id < m_format_sparse_set.dense.num_elements(),
                 "Error: new text slot id is larger than GpuBuffer size.");

    if (!m_format_sparse_set.available.empty()) {
        m_format_sparse_set.available.pop_back();
    }

    m_format_sparse_set.sparse[format_id] = m_format_sparse_set.dense_count++;
    m_format_sparse_set.reverse.push_back(format_id);

    return graphics_pipeline::text::TextFormatSBOHandle(format_id);
}

graphics_pipeline::text::TextGlyphSBOHandle
graphics_pipeline::text::TextRenderer::request_glyph_slot() {
    size_t glyph_id = m_glyph_sparse_set.available.empty()
                          ? m_glyph_sparse_set.next_id++
                          : m_glyph_sparse_set.available.back();

    DEBUG_ASSERT(glyph_id < m_glyph_sparse_set.dense.num_elements(),
                 "Error: new glyph slot id is larger than GpuBuffer size.");

    if (!m_glyph_sparse_set.available.empty()) {
        m_glyph_sparse_set.available.pop_back();
    }

    m_glyph_sparse_set.sparse[glyph_id] = m_glyph_sparse_set.dense_count++;
    m_glyph_sparse_set.reverse.push_back(glyph_id);

    return graphics_pipeline::text::TextGlyphSBOHandle(glyph_id);
}

void graphics_pipeline::text::TextRenderer::return_format_slot(
    graphics_pipeline::text::TextFormatSBOHandle &handle) {
    const size_t format_id = handle.id;
    if (contains_format(format_id)) {
        size_t dense_index = m_format_sparse_set.sparse[format_id];
        size_t last_dense_index = m_format_sparse_set.dense_count - 1;
        size_t last_id = m_format_sparse_set.reverse[last_dense_index];

        if (dense_index != last_dense_index) {
            m_format_sparse_set.dense[dense_index] =
                std::move(m_format_sparse_set.dense[last_dense_index]);
            m_format_sparse_set.reverse[dense_index] = last_id;
            m_format_sparse_set.sparse[last_id] = dense_index;
        }

        m_format_sparse_set.dense[last_dense_index] = TextFormatSBO{};
        m_format_sparse_set.reverse.pop_back();
        m_format_sparse_set.sparse[format_id] = INVALID_INDEX;
        m_format_sparse_set.available.push_back(format_id);
        m_format_sparse_set.dense_count--;
    }
}

void graphics_pipeline::text::TextRenderer::return_glyph_slot(
    graphics_pipeline::text::TextGlyphSBOHandle &handle) {
    const size_t glyph_id = handle.id;
    if (contains_glyph(glyph_id)) {
        size_t dense_index = m_glyph_sparse_set.sparse[glyph_id];
        size_t last_dense_index = m_glyph_sparse_set.dense_count - 1;
        size_t last_id = m_glyph_sparse_set.reverse[last_dense_index];

        if (dense_index != last_dense_index) {
            m_glyph_sparse_set.dense[dense_index] =
                std::move(m_glyph_sparse_set.dense[last_dense_index]);
            m_glyph_sparse_set.reverse[dense_index] = last_id;
            m_glyph_sparse_set.sparse[last_id] = dense_index;
        }

        m_glyph_sparse_set.dense[last_dense_index] = TextGlyphSBO{};
        m_glyph_sparse_set.reverse.pop_back();
        m_glyph_sparse_set.sparse[glyph_id] = INVALID_INDEX;
        m_glyph_sparse_set.available.push_back(glyph_id);
        m_glyph_sparse_set.dense_count--;
    }
}

void graphics_pipeline::text::TextRenderer::sync_render_slots() {
    m_format_sparse_set.dense.sync_all();
    m_glyph_sparse_set.dense.sync_all();
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
}

graphics_pipeline::text::TextHandle
graphics_pipeline::text::TextRenderer::create_text(const font::Unicode &codepoint,
                                                   const TextOpts &opts) {

    if (!is_font_loaded()) {
        throw std::runtime_error(
            "Error: can't create text because a font is not loaded.");
    }

    TextFormatSBOHandle format_handle = request_format_slot();
    TextFormatSBO &format_instance = get_text_format_instance(format_handle.id);
    format_instance.model_matrix = math::Matrix().translate(opts.position);
    format_instance.font_color = opts.font_color;

    std::vector<uint32_t> index_count;
    std::vector<uint32_t> first_index;

    index_count.reserve(codepoint.size());
    first_index.reserve(codepoint.size());

    const font::FontBBox bbox = m_font_loader->get_font_bbox();
    const unsigned short units_per_em = m_font_loader->get_units_per_em();
    const float glyph_width = bbox.x_max - bbox.x_min;
    const float font_scale = static_cast<float>(opts.font_size) / units_per_em;
    const float font_size_px = glyph_width * font_scale;

    float pen_position_x = 0.0f;

    TextGlyphSBOHandle first_glyph_handle = request_glyph_slot();
    for (size_t i = 0; i < codepoint.size(); i++) {
        const char32_t &c = codepoint[i];
        // Glyph id in terms of the text (not font)
        request_glyph_slot();
        size_t glyph_id = first_glyph_handle.id + i;

        m_glyph_sparse_set.dense[m_glyph_sparse_set.sparse[glyph_id]] =
            TextGlyphSBO{.text_id = static_cast<uint16_t>(format_handle.id),
                         .model_matrix = math::Matrix()
                                             .scale(font_scale, font_scale, 1.0f)
                                             .translate(pen_position_x, 0.0f, 0.0f)

            };

        std::pair<size_t, size_t> draw_info =
            m_glyph_draw_info[m_font_loader->get_glyph_index(c)];
        index_count.push_back(draw_info.first);
        first_index.push_back(draw_info.second);

        const font::GlyphAdvance &advance = m_font_loader->get_glyph_advance(c);
        pen_position_x += advance.x;

        if (i < codepoint.size() - 1) {
            font::GlyphKerning kerning =
                m_font_loader->get_glyph_kerning(codepoint[i], codepoint[i + 1]);
            pen_position_x += kerning.x;
        }
    }

    sync_render_slots();

    TextHandle result;
    result.format_handle = std::move(format_handle);
    result.glyph_handle = std::move(first_glyph_handle);
    result.glyph_count = static_cast<uint32_t>(codepoint.size());
    result.index_count = std::move(index_count);
    result.first_index = std::move(first_index);

    return result;
}

void graphics_pipeline::text::TextRenderer::remove_text(TextHandle &handle) {
    return_format_slot(handle.format_handle);
    uint32_t first_glyph_id = handle.glyph_handle.id;
    for (uint32_t i = 0; i < handle.glyph_count; i++) {
        TextGlyphSBOHandle glyph_handle = TextGlyphSBOHandle(first_glyph_id + i);
        return_glyph_slot(glyph_handle);
    }
}

graphics_pipeline::text::TextHandle
graphics_pipeline::text::TextRenderer::get_font_showcase_text() {
    if (!is_font_loaded()) {
        throw std::runtime_error(
            "Error: Can't render font showcase because font is not loaded.");
    }

    const size_t num_glyphs = m_font_loader->get_num_glyphs();

    TextFormatSBOHandle format_handle = request_format_slot();
    TextFormatSBO &format_instance = get_text_format_instance(format_handle.id);
    format_instance.model_matrix = math::Matrix();

    std::vector<uint32_t> index_count;
    std::vector<uint32_t> first_index;

    index_count.reserve(num_glyphs);
    first_index.reserve(num_glyphs);

    const font::FontBBox bbox = m_font_loader->get_font_bbox();
    const float column_width = bbox.x_max - bbox.x_min;
    const float column_height = bbox.x_max - bbox.x_min;

    const size_t num_cols = 8;
    TextGlyphSBOHandle first_glyph_handle = request_glyph_slot();
    for (size_t gid = 0; gid < num_glyphs; gid++) {

        const float x = column_width * (gid % num_cols);
        const float y = column_height * (static_cast<float>(gid) / num_cols);

        request_glyph_slot();
        size_t glyph_id = first_glyph_handle.id + gid;
        m_glyph_sparse_set.dense[m_glyph_sparse_set.sparse[glyph_id]] = TextGlyphSBO{
            .text_id = static_cast<uint16_t>(format_handle.id),
            .model_matrix = math::Matrix().translate(x, y, 0.0f).scale(0.1),
        };

        const std::pair<size_t, size_t> glyph_info = m_glyph_draw_info[gid];
        index_count.push_back(glyph_info.first);
        first_index.push_back(glyph_info.second);
    }

    sync_render_slots();

    TextHandle result;
    result.format_handle = std::move(format_handle);
    result.glyph_handle = std::move(first_glyph_handle);
    result.glyph_count = static_cast<uint32_t>(num_glyphs);
    result.index_count = std::move(index_count);
    result.first_index = std::move(first_index);

    return result;
}

void graphics_pipeline::text::TextRenderer::_render(
    const vulkan::CommandBuffer &command_buffer, const TextHandle &text) {
    for (size_t i = 0; i < text.glyph_count; i++) {
        vkCmdDrawIndexed(command_buffer, text.index_count[i], 1, text.first_index[i], 0,
                         text.glyph_handle.id + i);
    }
}
