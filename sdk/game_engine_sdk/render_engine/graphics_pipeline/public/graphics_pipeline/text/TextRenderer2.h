#pragma once

#include "font/FontLoader.h"
#include "font/Unicode.h"
#include "graphics_pipeline/RendererOpts.h"
#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "graphics_pipeline/text/TextOpts.h"
#include "graphics_pipeline/text/TextPipeline.h"
#include "math/Vector2.h"
#include "util/colors.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/DescriptorSetLayout.h"
#include "vulkan/DrawIndexedIndirectCommand.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"

namespace graphics_pipeline::text {

struct Word;
struct Text;

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

// CONTINUE: Refactoring steps:
// 1. DONE Break out text formatting to its own class.
// 2. DONE Make use of the new text formatting class in the existing TextRenderer.
// 3. Implement the new TextRenderer2 class.
// 4. Transition IntroState to use the TextRenderer2 class.

class TextRenderer2 {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    TextPipeline m_text_pipeline;

    std::optional<font::FontLoader> m_font_loader;
    std::optional<vulkan::buffers::VertexBuffer<GlyphVertex>> m_glyph_vertex_buffer;
    std::optional<vulkan::buffers::IndexBuffer> m_glyph_index_buffer;

    std::vector<std::pair<size_t, size_t>> m_glyph_draw_info;

    vulkan::buffers::StorageBuffer<TextFormatSBO> m_text_format_instances;
    vulkan::buffers::StorageBuffer<TextGlyphSBO> m_glyph_instances;
    vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand> m_draw_commands;

    vulkan::DescriptorPool m_descriptor_pool;
    SwapDescriptorSet m_descriptor_sets;

    bool contains_format(size_t id) const;
    bool contains_glyph(size_t id) const;

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);
    void allocate_descriptor_set();

    // text formatting
    bool ends_with(const font::Unicode &codepoint, const Word &word,
                   const char character);
    Word layout_word(const font::Unicode &codepoint, const size_t start, const size_t end,
                     const math::Vector2 &offset);
    Text layout_text(const font::Unicode &codepoint, const TextOpts &opts);

  public:
    TextRenderer2() = default;
    TextRenderer2(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                  const RendererOpts &opts);

    TextRenderer2(TextRenderer2 &&) noexcept = default;

    bool is_font_loaded() { return m_font_loader.has_value(); }
    void load_font(vulkan::CommandBufferManager *command_buffer_manager,
                   font::FontLoader &&font_loader);

    void sync_render_slots();
    void rotate_descriptors();
};

} // namespace graphics_pipeline::text
