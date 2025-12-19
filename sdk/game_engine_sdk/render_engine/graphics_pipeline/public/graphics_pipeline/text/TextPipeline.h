#pragma once
#include "TextPipelineDescriptorSet.h"
#include "font/OTFFont.h"
#include "vulkan/CommandBuffer.h"
#include "vulkan/DrawIndirectCommand.h"
#include "vulkan/Pipeline.h"
#include "vulkan/PipelineLayout.h"
#include "vulkan/PushConstantRange.h"
#include "vulkan/ShaderStage.h"
#include "vulkan/buffers/IndexBuffer.h"
#include "vulkan/buffers/VertexBuffer.h"
#include "vulkan/context/GraphicsContext.h"
#include <memory>

namespace graphics_pipeline::text {

class TextPipeline {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

    std::optional<vulkan::buffers::VertexBuffer> m_glyph_vertex_buffer;
    std::optional<vulkan::buffers::IndexBuffer> m_glyph_index_buffer;

    std::optional<vulkan::buffers::StorageBuffer<vulkan::DrawIndirectCommand>>
        m_draw_command_buffer;

    std::optional<font::OTFFont> m_font;

    vulkan::ShaderStageFlags m_push_constant_stage;

    vulkan::PipelineLayout m_pipeline_layout;
    vulkan::Pipeline m_pipeline;

  public:
    TextPipeline(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                 vulkan::CommandBufferManager *command_buffer_manager,
                 vulkan::SwapChainManager *swap_chain_manager,
                 const vulkan::DescriptorSetLayout *descriptor_set_layout,
                 const vulkan::PushConstantRange *push_constant_range);

    void load_font(vulkan::CommandBufferManager *command_buffer_manager,
                   const font::OTFFont &font);

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

        const int num_draw_calls = 1;
        const int stride = sizeof(vulkan::DrawIndirectCommand);
        const auto glyph_id = m_font->glyph_index(unicode);

        auto draw_command_buffer_ref = m_draw_command_buffer->get_reference();
        const int offset = glyph_id * stride;
        vkCmdDrawIndirect(command_buffer, draw_command_buffer_ref.buffer, offset,
                          num_draw_calls, stride);
    }
};

} // namespace graphics_pipeline::text
