#pragma once

#include "font/Font.h"
#include "graphics_pipeline/RendererOpts.h"
#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "graphics_pipeline/text/TextPipeline.h"
#include "util/colors.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/DescriptorSetLayout.h"
#include "vulkan/DrawIndexedIndirectCommand.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"

namespace graphics_pipeline::text {

// Glyph specific data like
// - kerning information
struct TextGlyphSBO2 {
    alignas(4) uint32_t text_id = 0; // Which text this glyph belongs to
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
};

// Common data for the entire string, like
// - text color
// - font size
// - text position
struct TextFormatSBO2 {
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

    vulkan::buffers::StorageBuffer<TextFormatSBO2> m_text_format_instances;
    vulkan::buffers::StorageBuffer<TextGlyphSBO2> m_glyph_instances;
    vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand> m_draw_commands;

    TextPipeline m_text_pipeline;

    std::optional<vulkan::buffers::VertexBuffer<GlyphVertex>> m_glyph_vertex_buffer;
    std::optional<vulkan::buffers::IndexBuffer> m_glyph_index_buffer;

    font::Font m_font;

    std::vector<std::pair<size_t, size_t>> m_glyph_draw_info;

    vulkan::DescriptorPool m_descriptor_pool;
    SwapDescriptorSet m_descriptor_sets;

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

  public:
    TextRenderer2() = default;
    TextRenderer2(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                  const RendererOpts &opts);

    TextRenderer2(TextRenderer2 &&) noexcept = default;

    bool is_font_loaded();
    void load_font(vulkan::CommandBufferManager *command_buffer_manager,
                   const std::string &font_path);

    void write_to_format_buffer(const std::vector<TextFormatSBO2> &instance_data,
                                const size_t offset = 0);

    std::vector<vulkan::DrawIndexedIndirectCommand>
    write_to_glyph_buffer(const std::vector<TextGlyphSBO2> &instance_data,
                          const size_t offset = 0);
};

} // namespace graphics_pipeline::text
