#pragma once

#include "GraphicsPipeline.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/descriptors/DescriptorSetLayout.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include "vulkan/vulkan_core.h"
#include <optional>

// std::unique_ptr<GeometryPipeline> geometry_pipeline = PipelineBuilder()
// // If index buffer is set use index based drawing
// .set_vertex_shader(<vertex shader>)
// .set_fragment_shader(<fragment shader>)
// .set_vertex_buffer(<vertex buffer>)
// .set_index_buffer(<index buffer>)
// .set_push_constants(<push_constants>)
// .add_sampler(<sampler>)
// .add_texture(<texture>)
// .add_gpu_buffer(<gpu buffer>)
// .build(ctx, swap_chain_manager)
namespace graphics_pipeline {

class GraphicsPipelineBuilder {
  private:
    const ShaderResource *m_vertex_shader_resource;
    const ShaderResource *m_fragment_shader_resource;
    std::optional<VkPushConstantRange> m_push_constant_range;

    DescriptorSetLayout *m_descriptor_set_layout; // Temporary

  public:
    GraphicsPipelineBuilder() = default;
    ~GraphicsPipelineBuilder() = default;

    GraphicsPipelineBuilder &set_vertex_shader(const ShaderResource *value);
    GraphicsPipelineBuilder &set_fragment_shader(const ShaderResource *value);
    GraphicsPipelineBuilder &set_push_constants(VkPushConstantRange &&value);

    // TODO: Think about destruction
    // TODO: This function is temporary and should instead be created during build() based
    // on the values from the other set() functions
    GraphicsPipelineBuilder &set_descriptor_set_layout(DescriptorSetLayout *value);

    GraphicsPipeline build(std::shared_ptr<graphics_context::GraphicsContext> &ctx,
                           SwapChainManager &swap_chain_manager);
};
} // namespace graphics_pipeline
