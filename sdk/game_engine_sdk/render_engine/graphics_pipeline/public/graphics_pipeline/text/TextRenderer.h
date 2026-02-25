#pragma once

#include "font/FontLoader.h"
#include "font/Unicode.h"
#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "graphics_pipeline/text/TextPipeline.h"
#include "math/Vector2.h"
#include "util/colors.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/DescriptorSetLayout.h"
#include "vulkan/SwapChainManager.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include <limits>

namespace graphics_pipeline::text {

// Glyph specific data like
// - kerning information
struct GlyphSBO {
    alignas(4) uint32_t text_id; // Which text this glyph belongs to
    alignas(16) glm::mat4 offset = glm::mat4(1.0f);
};

// Common data for the entire string, like
// - text color
// - font size
// - text position
struct TextSBO {
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
    alignas(16) glm::vec4 color = util::colors::WHITE;
};

struct TextOpts {
    math::Vector2 position = math::Vector2(0, 0);
    util::colors::Color font_color = util::colors::WHITE;
    size_t font_size = 11;
};

class TextRenderer;

class TextSBOHandle {
  private:
    friend class TextRenderer;
    size_t id = std::numeric_limits<size_t>::max();

  public:
    TextSBOHandle() = default;
    TextSBOHandle(size_t id) : id(id) {}

    TextSBOHandle(const TextSBOHandle &) = delete;
    TextSBOHandle(TextSBOHandle &&other) noexcept = default;

    TextSBOHandle &operator=(const TextSBOHandle &&) = delete;
    TextSBOHandle &operator=(TextSBOHandle &&other) noexcept = default;
};

class GlyphSBOHandle {
  private:
    friend class TextRenderer;
    size_t id = std::numeric_limits<size_t>::max();

  public:
    GlyphSBOHandle() = default;
    GlyphSBOHandle(size_t id) : id(id) {}

    GlyphSBOHandle(const GlyphSBOHandle &) = delete;
    GlyphSBOHandle(GlyphSBOHandle &&other) noexcept = default;

    GlyphSBOHandle &operator=(const GlyphSBOHandle &&) = delete;
    GlyphSBOHandle &operator=(GlyphSBOHandle &&other) noexcept = default;
};

class TextStringHandle {
  private:
    friend class TextRenderer;
    TextSBOHandle m_text_handle;
    GlyphSBOHandle m_glyph_handle;
    size_t m_glyph_count = 0;

  public:
    TextSBOHandle &text_handle() { return m_text_handle; }
    GlyphSBOHandle &glyph_handle() { return m_glyph_handle; }
    size_t glyph_count() const { return m_glyph_count; }
};

class TextString {
  public:
    TextSBOHandle text_handle;
    GlyphSBOHandle glyph_handle;
    uint32_t glyph_count = 0;

    std::vector<uint32_t> index_count;
    std::vector<uint32_t> first_index;
};

class TextRenderer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    TextPipeline m_text_pipeline;

    std::optional<font::FontLoader> m_font_loader;
    std::optional<vulkan::buffers::VertexBuffer<GlyphVertex>> m_glyph_vertex_buffer;
    std::optional<vulkan::buffers::IndexBuffer> m_glyph_index_buffer;

    std::vector<std::pair<size_t, size_t>> m_glyph_draw_info;

    struct TextSparseSet {
        vulkan::buffers::StorageBuffer<TextSBO> dense;
        size_t next_id = 0;
        size_t dense_count = 0;
        std::vector<size_t> sparse;
        std::vector<size_t> reverse;
        std::vector<size_t> available;
    } m_text_sparse_set;

    struct {
        vulkan::buffers::StorageBuffer<GlyphSBO> dense;
        size_t next_id = 0;
        size_t dense_count = 0;
        std::vector<size_t> sparse;
        std::vector<size_t> reverse;
        std::vector<size_t> available;
    } m_glyph_sparse_set;

    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

    vulkan::DescriptorPool m_descriptor_pool;
    SwapDescriptorSet m_descriptor_sets;

    bool contains_text(size_t id) const;
    bool contains_glyph(size_t id) const;

    TextSBO &get_text_instance(const TextSBOHandle &handle);
    GlyphSBO &get_glyph_instance(const GlyphSBOHandle &handle);

    TextStringHandle request_text_slot(size_t num_glyphs);
    void return_text_slot(TextStringHandle &handle);

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

    void _render(const vulkan::CommandBuffer &command_buffer, const TextString &text);
    void allocate_descriptor_set();

  public:
    TextRenderer() = default;
    TextRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                 vulkan::SwapChainManager *swap_chain_manager,
                 const VkPushConstantRange *push_constant_range);

    TextRenderer(TextRenderer &&) noexcept = default;

    TextString get_font_showcase_text();
    TextString create_text(const font::Unicode &codepoint,
                           const TextOpts &opts = TextOpts{});

    bool is_font_loaded() { return m_font_loader.has_value(); }
    void load_font(vulkan::CommandBufferManager *command_buffer_manager,
                   font::FontLoader &&font_loader);

    void sync_render_slots();

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer, const TextString &text,
                PushConstantType *push_constant) {
        if (!is_font_loaded()) {
            throw std::runtime_error(
                "Error: can't render text because a font is not loaded.");
        }

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_text_pipeline.get_layout(),
                               m_text_pipeline.get_push_constant_stage(), 0,
                               sizeof(*push_constant), push_constant);
        }

        // Bind descriptors
        const vulkan::DescriptorSet set = m_descriptor_sets.get_current();
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_text_pipeline.get_layout(), 0, 1, &set, 0, nullptr);

        const VkDeviceSize vertex_offset = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_glyph_vertex_buffer->buffer,
                               &vertex_offset);
        vkCmdBindIndexBuffer(command_buffer, m_glyph_index_buffer->buffer, 0,
                             VK_INDEX_TYPE_UINT16);

        m_text_pipeline.bind(command_buffer);

        _render(command_buffer, text);
    }
};
} // namespace graphics_pipeline::text
