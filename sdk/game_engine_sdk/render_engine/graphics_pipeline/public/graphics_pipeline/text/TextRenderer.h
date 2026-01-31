#pragma once

#include "font/FontLoader.h"
#include "font/Unicode.h"
#include "graphics_pipeline/SwapDescriptorSet.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "graphics_pipeline/text/TextPipeline.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/DescriptorPool.h"
#include "vulkan/DescriptorSetLayout.h"
#include "vulkan/SwapChainManager.h"
#include "vulkan/buffers/GpuBuffer.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"

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
};

// CONTINUE: Make updatable (make a class)
struct TextString {
    uint32_t first_glyph;
    uint32_t glyph_count;

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

    vulkan::buffers::StorageBuffer<TextSBO> m_text_instances;
    vulkan::buffers::StorageBuffer<GlyphSBO> m_glyph_instances;

    vulkan::DescriptorPool m_descriptor_pool;
    SwapDescriptorSet m_descriptor_sets;

    size_t m_text_count = 0;
    size_t m_glyph_count = 0;

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
    TextString create_text(const font::Unicode &codepoint);

    bool is_font_loaded() { return m_font_loader.has_value(); }
    void load_font(vulkan::CommandBufferManager *command_buffer_manager,
                   font::FontLoader &&font_loader);

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

        // Bind geometry
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
