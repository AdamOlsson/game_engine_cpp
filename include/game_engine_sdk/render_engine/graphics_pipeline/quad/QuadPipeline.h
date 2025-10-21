#pragma once
#include "game_engine_sdk/render_engine/buffers/IndexBuffer.h"
#include "game_engine_sdk/render_engine/buffers/VertexBuffer.h"
#include "game_engine_sdk/render_engine/graphics_context/GraphicsContext.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipelineDescriptorSet.h"
#include "game_engine_sdk/render_engine/vulkan/CommandBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/Pipeline.h"
#include "game_engine_sdk/render_engine/vulkan/PipelineLayout.h"
#include "game_engine_sdk/render_engine/vulkan/PushConstantRange.h"
#include "game_engine_sdk/render_engine/vulkan/ShaderStage.h"
#include <memory>

namespace graphics_pipeline {

class QuadPipeline {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    VertexBuffer m_quad_vertex_buffer;
    IndexBuffer m_quad_index_buffer;

    vulkan::ShaderStageFlags m_push_constant_stage;

    vulkan::PipelineLayout m_pipeline_layout;
    vulkan::Pipeline m_pipeline;

  public:
    QuadPipeline(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 CommandBufferManager *command_buffer_manager,
                 SwapChainManager *swap_chain_manager,
                 const vulkan::DescriptorSetLayout *descriptor_set_layout,
                 const vulkan::PushConstantRange *push_constant_range);

    /*static QuadPipeline create(std::shared_ptr<graphics_context::GraphicsContext> ctx,*/
    /*                           CommandBufferManager *command_buffer_manager,*/
    /*                           SwapChainManager *swap_chain_manager);*/

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer,
                QuadPipelineDescriptorSet *descriptor_set,
                PushConstantType *push_constant, const int num_instances) {

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

        if (push_constant) {
            vkCmdPushConstants(command_buffer, m_pipeline_layout, m_push_constant_stage,
                               0, sizeof(*push_constant), push_constant);
        }

        if (descriptor_set) {
            const vulkan::DescriptorSet set = descriptor_set->get_next();
            vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_pipeline_layout, 0, 1, &set, 0, nullptr);
        }

        const VkDeviceSize vertex_buffers_offset = 0;
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_quad_vertex_buffer.buffer,
                               &vertex_buffers_offset);
        vkCmdBindIndexBuffer(command_buffer, m_quad_index_buffer.buffer, 0,
                             VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command_buffer, m_quad_index_buffer.num_indices, num_instances,
                         0, 0, 0);
    }
};

} // namespace graphics_pipeline
