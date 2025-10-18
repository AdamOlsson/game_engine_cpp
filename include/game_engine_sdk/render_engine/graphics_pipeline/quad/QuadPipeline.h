#pragma once
#include "game_engine_sdk/render_engine/buffers/IndexBuffer.h"
#include "game_engine_sdk/render_engine/buffers/VertexBuffer.h"
#include "game_engine_sdk/render_engine/graphics_context/GraphicsContext.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/quad/QuadPipelineDescriptorSet.h"
#include "game_engine_sdk/render_engine/vulkan/CommandBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/Pipeline.h"
#include "game_engine_sdk/render_engine/vulkan/PipelineLayout.h"
#include <memory>

namespace graphics_pipeline {

struct QuadPipelineUBO {
    std::string to_string() const {
        return std::format("QuadPipelineUBO {{\n"
                           "}}");
    }

    friend std::ostream &operator<<(std::ostream &os, const QuadPipelineUBO &obj) {
        return os << obj.to_string();
    }
};

class QuadPipeline {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

    VertexBuffer m_quad_vertex_buffer;
    IndexBuffer m_quad_index_buffer;

    vulkan::PipelineLayout m_pipeline_layout;
    vulkan::Pipeline m_pipeline;

  public:
    QuadPipeline(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                 CommandBufferManager *command_buffer_manager,
                 SwapChainManager *swap_chain_manager,
                 const std::optional<vulkan::DescriptorSetLayout> &descriptor_set_layout);

    void render(const vulkan::CommandBuffer &command_buffer,
                std::optional<QuadPipelineDescriptorSet> &descriptor_set,
                const int num_instances);
};

} // namespace graphics_pipeline
