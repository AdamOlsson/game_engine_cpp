#pragma once
#include "TextPipelineDescriptorSet.h"
#include "font/FontLoader.h"
#include "graphics_pipeline/text/GlyphVertex.h"
#include "vulkan/CommandBuffer.h"
#include "vulkan/DrawIndexedIndirectCommand.h"
#include "vulkan/Pipeline.h"
#include "vulkan/PipelineLayout.h"
#include "vulkan/PushConstantRange.h"
#include "vulkan/ShaderStage.h"
#include "vulkan/SwapChainManager.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include "vulkan/context/GraphicsContext.h"
#include <memory>

namespace graphics_pipeline::text {

class TextPipeline {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    std::optional<vulkan::buffers::VertexBuffer<GlyphVertex>> m_glyph_vertex_buffer;
    std::optional<vulkan::buffers::IndexBuffer> m_glyph_index_buffer;

    std::optional<vulkan::buffers::StorageBuffer<vulkan::DrawIndexedIndirectCommand>>
        m_draw_command_buffer;

    std::optional<font::FontLoader> m_font_loader;

    vulkan::ShaderStageFlags m_push_constant_stage;

    vulkan::PipelineLayout m_pipeline_layout;
    vulkan::Pipeline m_pipeline;

  public:
    TextPipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 vulkan::SwapChainManager *swap_chain_manager,
                 const vulkan::DescriptorSetLayout *descriptor_set_layout,
                 const vulkan::PushConstantRange *push_constant_range);

    void load_font(vulkan::CommandBufferManager *command_buffer_manager,
                   font::FontLoader &&font_laoder);

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer,
                TextPipelineDescriptorSet *descriptor_set,
                PushConstantType *push_constant, const font::Unicode &unicode) {

        DEBUG_ASSERT(m_glyph_vertex_buffer.has_value(),
                     "Error: can't render text because a font is not loaded");

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_pipeline_layout, m_push_constant_stage,
                               0, sizeof(*push_constant), push_constant);
        }

        if (descriptor_set) {
            const vulkan::DescriptorSet set = descriptor_set->get_current();
            vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_pipeline_layout, 0, 1, &set, 0, nullptr);
        }

        const VkDeviceSize vertex_buffers_offset = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_glyph_vertex_buffer->buffer,
                               &vertex_buffers_offset);
        vkCmdBindIndexBuffer(command_buffer, m_glyph_index_buffer->buffer, 0,
                             VK_INDEX_TYPE_UINT16);

        const int num_draw_calls = 1;
        const int stride = m_draw_command_buffer->size_of_T();
        const auto glyph_id = m_font_loader->get_glyph_index(unicode);

        auto draw_command_buffer_ref = m_draw_command_buffer->get_reference();

        const int offset = glyph_id * stride;

        vkCmdDrawIndexedIndirect(command_buffer, draw_command_buffer_ref.buffer, offset,
                                 num_draw_calls, stride);
    }

    template <typename PushConstantType>
    void render_font_showcase(const vulkan::CommandBuffer &command_buffer,
                              TextPipelineDescriptorSet *descriptor_set,
                              PushConstantType *push_constant) {

        DEBUG_ASSERT(m_glyph_vertex_buffer.has_value(),
                     "Error: can't render font showcase because a font is not loaded");

        for (const auto glyph_index : *m_font_loader) {
            render(command_buffer, descriptor_set, push_constant,
                   font::Unicode(glyph_index));
        }
    }
};

} // namespace graphics_pipeline::text
