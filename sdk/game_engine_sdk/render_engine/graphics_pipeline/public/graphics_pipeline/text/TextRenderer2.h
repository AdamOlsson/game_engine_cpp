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

struct TextGlyphSBO2 {
    alignas(4) uint32_t text_id = 0; // Which text this glyph belongs to
    alignas(16) glm::vec3 offset = glm::vec3(0.0f);
};

struct TextFormatSBO2 {
    alignas(16) glm::vec3 position = glm::vec3(1.0f);
    alignas(16) glm::vec4 font_color = util::colors::WHITE;
    alignas(4) float font_size = 11.0f;
};

// CONTINUE: Refactoring steps:
// 1. DONE Break out text formatting to its own class.
// 2. DONE Make use of the new text formatting class in the existing TextRenderer.
// 3. DONE Implement the new TextRenderer2 class.
// 3.5 DONE Create a nice interface for rendering text with TextRenderer2
// 4. DONE Transition IntroState to use the TextRenderer2 class.
// 4.5 Refactor all other usage of TextRenderer to use TextRenderer2
// 5. Evaluate if I can move font out from the TextRenderer2 class.
// 6. Improve TextRenderer2 draw call by instancing on each character.

class TextRenderer2 {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    vulkan::buffers::StorageBuffer<TextFormatSBO2> m_text_format_instances;
    vulkan::buffers::StorageBuffer<TextGlyphSBO2> m_glyph_instances;
    vulkan::buffers::IndirectBuffer<vulkan::DrawIndexedIndirectCommand> m_draw_commands;

    TextPipeline m_text_pipeline;

    std::optional<vulkan::buffers::VertexBuffer<GlyphVertex>> m_glyph_vertex_buffer;
    std::optional<vulkan::buffers::IndexBuffer> m_glyph_index_buffer;

    vulkan::DescriptorPool m_descriptor_pool;
    SwapDescriptorSet m_descriptor_sets;

    static vulkan::DescriptorSetLayout
    get_descriptor_set_layout(std::shared_ptr<vulkan::context::GraphicsContext> &ctx);

  public:
    font::Font m_font;

    TextRenderer2() = default;
    TextRenderer2(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                  const RendererOpts &opts);

    TextRenderer2(TextRenderer2 &&) noexcept = default;

    void load_font(vulkan::CommandBufferManager *command_buffer_manager,
                   const std::string &font_filepath);

    /*void write_to_format_buffer(const font::TextFormat &format, const size_t offset =
     * 0);*/
    void write_to_format_buffer(const std::vector<font::TextFormat> &format,
                                const size_t offset = 0);

    std::vector<vulkan::DrawIndexedIndirectCommand>
    write_to_glyph_buffer(const font::Text &text, const size_t format_id,
                          const size_t offset = 0);

    template <typename PushConstantType>
    void render_indirect(
        const vulkan::CommandBuffer &command_buffer, PushConstantType *push_constant,
        const std::vector<vulkan::DrawIndexedIndirectCommand> &draw_commands) {

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_text_pipeline.get_layout(),
                               m_text_pipeline.get_push_constant_stage(), 0,
                               sizeof(*push_constant), push_constant);
        }

        const vulkan::DescriptorSet set = m_descriptor_sets.get_current();
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_text_pipeline.get_layout(), 0, 1, &set, 0, nullptr);

        m_text_pipeline.bind(command_buffer);

        const VkDeviceSize vertex_offset = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_glyph_vertex_buffer->buffer,
                               &vertex_offset);
        vkCmdBindIndexBuffer(command_buffer, m_glyph_index_buffer->buffer, 0,
                             VK_INDEX_TYPE_UINT16);

        m_draw_commands.write(draw_commands);

        vkCmdDrawIndexedIndirect(command_buffer, m_draw_commands.handle(), 0,
                                 draw_commands.size(),
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
};

} // namespace graphics_pipeline::text
