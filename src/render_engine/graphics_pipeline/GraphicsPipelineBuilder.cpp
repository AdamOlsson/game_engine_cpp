#include "GraphicsPipelineBuilder.h"
#include "render_engine/ShaderModule.h"
#include "render_engine/vulkan/Pipeline.h"
#include <stdexcept>

graphics_pipeline::GraphicsPipelineBuilder &
graphics_pipeline::GraphicsPipelineBuilder::set_vertex_shader(
    const ShaderResource *value) {
    m_vertex_shader_resource = value;
    return *this;
}

graphics_pipeline::GraphicsPipelineBuilder &
graphics_pipeline::GraphicsPipelineBuilder::set_fragment_shader(
    const ShaderResource *value) {
    m_fragment_shader_resource = value;
    return *this;
}

graphics_pipeline::GraphicsPipelineBuilder &
graphics_pipeline::GraphicsPipelineBuilder::set_push_constants(
    VkPushConstantRange &&value) {
    m_push_constant_range = std::move(value);
    return *this;
}

graphics_pipeline::GraphicsPipelineBuilder &
graphics_pipeline::GraphicsPipelineBuilder::set_descriptor_set_layout(
    DescriptorSetLayout *value) {
    m_descriptor_set_layout = value;
    return *this;
}

graphics_pipeline::GraphicsPipeline graphics_pipeline::GraphicsPipelineBuilder::build(
    std::shared_ptr<graphics_context::GraphicsContext> &ctx,
    SwapChainManager &swap_chain_manager) {

    if (m_vertex_shader_resource == nullptr || m_fragment_shader_resource == nullptr) {
        throw std::runtime_error(
            "GraphicsPipelineBuilder::Vertex and fragment shaders need to be set");
    }

    // Temporary
    if (m_descriptor_set_layout == nullptr) {
        throw std::runtime_error(
            "GraphicsPipelineBuilder::Descriptor set layout needs to be set");
    }

    ShaderModule vertex_shader = ShaderModule(ctx, *m_vertex_shader_resource);
    ShaderModule fragment_shader = ShaderModule(ctx, *m_fragment_shader_resource);

    std::vector<VkPushConstantRange> push_constant_ranges = {};
    if (m_push_constant_range.has_value()) {
        push_constant_ranges.push_back(m_push_constant_range.value());
    }

    DescriptorSetLayout *descriptor_set_layout = m_descriptor_set_layout;
    vulkan::PipelineLayout layout =
        vulkan::PipelineLayout(ctx, *descriptor_set_layout, push_constant_ranges);
    vulkan::Pipeline pipeline = vulkan::Pipeline(ctx, &layout, vertex_shader,
                                                 fragment_shader, swap_chain_manager);
    return GraphicsPipeline(std::move(layout), std::move(pipeline));
}
