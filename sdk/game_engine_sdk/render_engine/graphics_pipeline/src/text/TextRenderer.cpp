#include "graphics_pipeline/text/TextRenderer.h"
#include "graphics_pipeline/DescriptorSetLayoutBuilder.h"
#include "graphics_pipeline/SwapDescriptorSetBuilder.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "graphics_pipeline/text/TextPipeline.h"
#include "math/Bbox.h"
#include "math/Matrix.h"
#include "vulkan/CommandBufferManager.h"

namespace graphics_pipeline::text {

TextRenderer::TextRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                           const RendererOpts &opts)
    : m_ctx(ctx) {

    graphics_pipeline::PipelineOpts pipeline_opts{};
    pipeline_opts.swap_chain.extent = opts.swap_chain.extent;
    pipeline_opts.swap_chain.render_pass = opts.swap_chain.render_pass;
    pipeline_opts.push_constant_range = opts.push_constant_range;
    pipeline_opts.descriptor.layout = TextRenderer::get_descriptor_set_layout(ctx);
    m_text_pipeline = TextPipeline(ctx, pipeline_opts);

    const size_t max_frames_in_flight = 2;
    constexpr size_t max_text_instances = 16;
    constexpr size_t max_glyph_instances = 1024;

    m_format_sparse_set.dense = vulkan::buffers::StagedStorageBuffer<TextFormatSBO>(
        ctx, max_text_instances, max_frames_in_flight);
    m_glyph_sparse_set.dense = vulkan::buffers::StagedStorageBuffer<TextGlyphSBO>(
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

vulkan::DescriptorSetLayout TextRenderer::get_descriptor_set_layout(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx) {
    graphics_pipeline::DescriptorSetLayoutBuilder builder;
    builder.add_storage_buffer_binding(0, VK_SHADER_STAGE_VERTEX_BIT);
    builder.add_storage_buffer_binding(1, VK_SHADER_STAGE_VERTEX_BIT);
    return builder.build(ctx);
}

bool TextRenderer::contains_format(size_t id) const {
    return id < m_format_sparse_set.sparse.size() &&
           m_format_sparse_set.sparse[id] != INVALID_INDEX;
}

bool TextRenderer::contains_glyph(size_t id) const {
    return id < m_glyph_sparse_set.sparse.size() &&
           m_glyph_sparse_set.sparse[id] != INVALID_INDEX;
}

TextFormatSBO &TextRenderer::get_text_format_instance(const TextHandle &handle) {
    return m_format_sparse_set.dense[m_format_sparse_set.sparse[handle.format_handle.id]];
}

TextFormatSBO &TextRenderer::get_text_format_instance(const TextFormatSBOHandle &handle) {
    return m_format_sparse_set.dense[m_format_sparse_set.sparse[handle.id]];
}

TextGlyphSBO &TextRenderer::get_text_glyph_instance(const TextGlyphSBOHandle &handle) {
    return m_glyph_sparse_set.dense[m_glyph_sparse_set.sparse[handle.id]];
}

TextFormatSBOHandle TextRenderer::request_format_slot() {

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

    return TextFormatSBOHandle(format_id);
}

TextGlyphSBOHandle TextRenderer::request_glyph_slot() {
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

    return TextGlyphSBOHandle(glyph_id);
}

void TextRenderer::return_format_slot(TextFormatSBOHandle &handle) {
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

void TextRenderer::return_glyph_slot(TextGlyphSBOHandle &handle) {
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

void TextRenderer::rotate_descriptors() {
    m_format_sparse_set.dense.rotate();
    m_glyph_sparse_set.dense.rotate();
    m_descriptor_sets.rotate();
}

void TextRenderer::sync_render_slots() {
    m_format_sparse_set.dense.sync();
    m_glyph_sparse_set.dense.sync();
}

void TextRenderer::load_font(vulkan::CommandBufferManager *command_buffer_manager,
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

void TextRenderer::remove_text(TextHandle &&handle) {
    return_format_slot(handle.format_handle);
    for (auto &glyph_handle : handle.glyph_handles) {
        return_glyph_slot(glyph_handle);
    }
}

TextHandle TextRenderer::get_font_showcase_text() {
    if (!is_font_loaded()) {
        throw std::runtime_error(
            "Error: Can't render font showcase because font is not loaded.");
    }

    const size_t num_glyphs = m_font.get_num_glyphs();

    TextFormatSBOHandle format_handle = request_format_slot();
    TextFormatSBO &format_instance = get_text_format_instance(format_handle.id);
    format_instance.model_matrix = math::Matrix();

    std::vector<uint32_t> index_count;
    std::vector<uint32_t> first_index;

    index_count.reserve(num_glyphs);
    first_index.reserve(num_glyphs);

    const font::FontBBox bbox = m_font.get_font_bbox();
    const float column_width = bbox.x_max - bbox.x_min;
    const float column_height = bbox.x_max - bbox.x_min;

    const size_t num_cols = 8;
    std::vector<TextGlyphSBOHandle> glyph_handles;
    glyph_handles.reserve(num_glyphs);
    for (size_t gid = 0; gid < num_glyphs; gid++) {

        const float x = column_width * (gid % num_cols);
        const float y = column_height * (static_cast<float>(gid) / num_cols);

        glyph_handles.push_back(request_glyph_slot());
        size_t glyph_id = glyph_handles.back().id;
        m_glyph_sparse_set.dense[m_glyph_sparse_set.sparse[glyph_id]] = TextGlyphSBO{
            .text_id = static_cast<uint16_t>(format_handle.id),
            .model_matrix = math::Matrix().translate(x, y, 0.0f).scale(0.1),
        };

        const std::pair<size_t, size_t> glyph_info = m_font.get_glyph_draw_info(gid);
        index_count.push_back(glyph_info.first);
        first_index.push_back(glyph_info.second);
    }

    sync_render_slots();

    TextHandle result;
    result.format_handle = std::move(format_handle);
    result.glyph_handles = std::move(glyph_handles);
    result.index_count = std::move(index_count);
    result.first_index = std::move(first_index);

    return result;
}

void TextRenderer::_render(const vulkan::CommandBuffer &command_buffer,
                           const TextHandle &text) {
    for (size_t i = 0; i < text.glyph_handles.size(); i++) {
        size_t dense_index = m_glyph_sparse_set.sparse[text.glyph_handles[i].id];
        vkCmdDrawIndexed(command_buffer, text.index_count[i], 1, text.first_index[i], 0,
                         dense_index);
    }
}

TextFormatSBOHandle TextRenderer::create_text_format_handle(const font::TextOpts &opts) {
    TextFormatSBOHandle format_handle = request_format_slot();
    TextFormatSBO &format_instance = get_text_format_instance(format_handle.id);
    format_instance.model_matrix = math::Matrix().translate(opts.position);
    format_instance.font_color = opts.font_color;
    return format_handle;
}

TextHandle TextRenderer::create_text2(const font::Unicode &codepoint,
                                      const font::TextOpts &opts) {

    if (!is_font_loaded()) {
        throw std::runtime_error(
            "Error: can't create text because a font is not loaded.");
    }

    font::Text text = m_font.format(codepoint, opts);

    const unsigned short units_per_em = m_font.get_units_per_em();
    const float font_scale = opts.font_size / units_per_em;

    TextHandle result;
    result.glyph_handles.reserve(text.char_count);
    result.index_count.reserve(text.char_count);
    result.first_index.reserve(text.char_count);

    result.bbox = text.bbox;
    result.bbox.offset(opts.position);

    TextFormatSBOHandle format_handle = create_text_format_handle(opts);
    result.format_handle = std::move(format_handle);

    for (const font::Word &word : text.words) {
        const size_t num_chars = word.end_idx - word.start_idx;
        for (size_t char_idx = 0; char_idx < num_chars; char_idx++) {
            const char32_t &c = codepoint[word.start_idx + char_idx];

            std::pair<size_t, size_t> draw_info = m_font.get_draw_info(c);

            result.index_count.push_back(draw_info.first);
            result.first_index.push_back(draw_info.second);

            const math::Vector2 &char_offset = word.glyph_positions[char_idx];
            TextGlyphSBOHandle glyph_handle = request_glyph_slot();
            TextGlyphSBO &glyph_instance = get_text_glyph_instance(glyph_handle);
            glyph_instance.text_id =
                static_cast<uint16_t>(m_format_sparse_set.sparse[format_handle.id]);
            glyph_instance.model_matrix = math::Matrix()
                                              .scale(font_scale, font_scale, 1.0f)
                                              .translate(word.offset + char_offset);
            result.glyph_handles.push_back(std::move(glyph_handle));
        }
    }

    return result;
}
} // namespace graphics_pipeline::text
