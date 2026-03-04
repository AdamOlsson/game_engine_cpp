#pragma once

#include "font/FontLoader.h"
#include "font/Unicode.h"
#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "graphics_pipeline/text/TextPipeline.h"
#include "math/Vector2.h"
#include "math/Vector4.h"
#include "math/shape.h"
#include "util/colors.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/DescriptorSetLayout.h"
#include "vulkan/SwapChain.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include <limits>

namespace graphics_pipeline::text {

// Glyph specific data like
// - kerning information
struct TextGlyphSBO {
    alignas(4) uint32_t text_id = 0; // Which text this glyph belongs to
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
};

// Common data for the entire string, like
// - text color
// - font size
// - text position
struct TextFormatSBO {
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
    alignas(16) glm::vec4 font_color = util::colors::WHITE;
};

struct TextOpts {
    math::Vector2 position = math::Vector2(0, 0);
    util::colors::Color font_color = util::colors::WHITE;
    float font_size = 11;
    float line_width = 100.0f;
    float line_height = 10.0f;
};

class TextRenderer;

class TextFormatSBOHandle {
  private:
    friend class TextRenderer;
    size_t id = std::numeric_limits<size_t>::max();

  public:
    TextFormatSBOHandle() = default;
    TextFormatSBOHandle(size_t id) : id(id) {}

    TextFormatSBOHandle(const TextFormatSBOHandle &) = delete;
    TextFormatSBOHandle(TextFormatSBOHandle &&other) noexcept = default;

    TextFormatSBOHandle &operator=(const TextFormatSBOHandle &&) = delete;
    TextFormatSBOHandle &operator=(TextFormatSBOHandle &&other) noexcept = default;
};

class TextGlyphSBOHandle {
  private:
    friend class TextRenderer;
    size_t id = std::numeric_limits<size_t>::max();

  public:
    TextGlyphSBOHandle() = default;
    TextGlyphSBOHandle(size_t id) : id(id) {}

    TextGlyphSBOHandle(const TextGlyphSBOHandle &) = delete;
    TextGlyphSBOHandle(TextGlyphSBOHandle &&other) noexcept = default;

    TextGlyphSBOHandle &operator=(const TextGlyphSBOHandle &&) = delete;
    TextGlyphSBOHandle &operator=(TextGlyphSBOHandle &&other) noexcept = default;
};

class TextHandle {
  private:
    friend class TextRenderer;

    TextFormatSBOHandle format_handle;
    std::vector<TextGlyphSBOHandle> glyph_handles;

    std::vector<uint32_t> index_count;
    std::vector<uint32_t> first_index;

    math::Vector4 bbox = math::Vector4(0.0f);

  public:
    TextHandle() = default;

    TextHandle(TextHandle &&other) noexcept = default;
    TextHandle(const TextHandle &other) = delete;

    TextHandle &operator=(TextHandle &&other) noexcept = default;
    TextHandle &operator=(const TextHandle &other) = delete;

    bool is_point_inside(const math::Vector2 &point) {
        const math::Vector2 center = (bbox.zw() - bbox.xy()) / 2.0f;
        const float width = abs(bbox.z() - bbox.x());
        const float height = abs(bbox.w() - bbox.y());
        std::cout << "Point: " << point << " center: " << center << " width: " << width
                  << " height: " << height << std::endl;
        return math::is_point_inside_rectangle(point, center, width, height);
    }
};

class TextRenderer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    TextPipeline m_text_pipeline;

    std::optional<font::FontLoader> m_font_loader;
    std::optional<vulkan::buffers::VertexBuffer<GlyphVertex>> m_glyph_vertex_buffer;
    std::optional<vulkan::buffers::IndexBuffer> m_glyph_index_buffer;

    std::vector<std::pair<size_t, size_t>> m_glyph_draw_info;

    struct {
        vulkan::buffers::StorageBuffer<TextFormatSBO> dense;
        size_t next_id = 0;
        size_t dense_count = 0;
        std::vector<size_t> sparse;
        std::vector<size_t> reverse;
        std::vector<size_t> available;
    } m_format_sparse_set;

    struct {
        vulkan::buffers::StorageBuffer<TextGlyphSBO> dense;
        size_t next_id = 0;
        size_t dense_count = 0;
        std::vector<size_t> sparse;
        std::vector<size_t> reverse;
        std::vector<size_t> available;
    } m_glyph_sparse_set;

    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

    vulkan::DescriptorPool m_descriptor_pool;
    SwapDescriptorSet m_descriptor_sets;

    bool contains_format(size_t id) const;
    bool contains_glyph(size_t id) const;

    TextGlyphSBO &get_text_glyph_instance(const TextGlyphSBOHandle &handle);

    TextFormatSBOHandle request_format_slot();
    TextGlyphSBOHandle request_glyph_slot();

    void return_format_slot(TextFormatSBOHandle &handle);
    void return_glyph_slot(TextGlyphSBOHandle &handle);

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

    void _render(const vulkan::CommandBuffer &command_buffer, const TextHandle &text);
    void allocate_descriptor_set();

  public:
    TextRenderer() = default;
    TextRenderer(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                 vulkan::SwapChain *swap_chain,
                 const VkPushConstantRange *push_constant_range);

    TextRenderer(TextRenderer &&) noexcept = default;

    TextHandle get_font_showcase_text();
    TextHandle create_text(const font::Unicode &codepoint,
                           const TextOpts &opts = TextOpts{});

    void remove_text(TextHandle &&handle);

    TextFormatSBO &get_text_format_instance(const TextHandle &handle);
    TextFormatSBO &get_text_format_instance(const TextFormatSBOHandle &handle);

    bool is_font_loaded() { return m_font_loader.has_value(); }
    void load_font(vulkan::CommandBufferManager *command_buffer_manager,
                   font::FontLoader &&font_loader);

    void sync_render_slots();

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer, const TextHandle &text,
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
