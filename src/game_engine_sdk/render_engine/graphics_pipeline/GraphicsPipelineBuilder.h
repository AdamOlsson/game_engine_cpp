#pragma once

#include "game_engine_sdk/render_engine/SwapChainManager.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GraphicsPipeline.h"
#include "game_engine_sdk/render_engine/resources/shaders/ShaderResource.h"
#include "game_engine_sdk/render_engine/vulkan/DescriptorSetLayout.h"
#include "game_engine_sdk/render_engine/vulkan/context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <optional>

namespace graphics_pipeline {

class GraphicsPipelineBuilder {
  private:
    const ShaderResource *m_vertex_shader_resource;
    const ShaderResource *m_fragment_shader_resource;
    std::optional<VkPushConstantRange> m_push_constant_range;

    vulkan::DescriptorSetLayout *m_descriptor_set_layout;

  public:
    GraphicsPipelineBuilder() = default;
    ~GraphicsPipelineBuilder() = default;

    GraphicsPipelineBuilder &set_vertex_shader(const ShaderResource *value);
    GraphicsPipelineBuilder &set_fragment_shader(const ShaderResource *value);
    GraphicsPipelineBuilder &set_push_constants(VkPushConstantRange &&value);

    GraphicsPipelineBuilder &
    set_descriptor_set_layout(vulkan::DescriptorSetLayout *value);

    GraphicsPipeline build(std::shared_ptr<vulkan::context::GraphicsContext> &ctx,
                           SwapChainManager &swap_chain_manager);
};
} // namespace graphics_pipeline
