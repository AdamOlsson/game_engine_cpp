#include "UIPipeline.h"
#include "render_engine/Geometry.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/ShaderModule.h"
#include "render_engine/descriptors/DescriptorSet.h"
#include "render_engine/descriptors/DescriptorSetLayoutBuilder.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include "vulkan/vulkan_core.h"

using namespace ui;

UIPipeline::UIPipeline(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                       SwapChainManager &swap_chain_manager,
                       SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers)
    : m_ctx(ctx),
      // TODO: Is it not better to merge all these to a single Descriptor class
      m_descriptor_set_layout(create_descriptor_set_layout()),
      m_descriptor_pool(DescriptorPool(m_ctx, m_descriptor_pool_capacity,
                                       m_num_storage_buffers, m_num_uniform_buffers,
                                       m_num_samplers)),
      m_descriptor_set(create_descriptor_set(uniform_buffers)),
      m_pipeline(create_pipeline(swap_chain_manager)),
      m_vertex_buffer(m_ctx, Geometry::rectangle_vertices, swap_chain_manager),
      m_index_buffer(IndexBuffer(ctx, Geometry::rectangle_indices, swap_chain_manager)) {}

UIPipeline::~UIPipeline() {}

void UIPipeline::render(const VkCommandBuffer &command_buffer,
                        const ElementProperties::ContainerProperties &ui_element) {
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_pipeline.m_pipeline);

    vkCmdPushConstants(command_buffer, m_pipeline.m_pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(ElementProperties::ContainerProperties), &ui_element);

    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer.buffer,
                           &vertex_buffers_offset);
    vkCmdBindIndexBuffer(command_buffer, m_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);

    auto descriptor = m_descriptor_set.get(); // TODO: This should be not be needed.
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipeline.m_pipeline_layout, 0, 1, &descriptor, 0, nullptr);

    vkCmdDrawIndexed(command_buffer, m_index_buffer.num_indices, 1, 0, 0, 0);
}

DescriptorSetLayout UIPipeline::create_descriptor_set_layout() {
    return DescriptorSetLayoutBuilder()
        .add(BufferDescriptor<
             GpuBufferType::Uniform>::create_descriptor_set_layout_binding(0))
        .build(m_ctx);
}

DescriptorSet UIPipeline::create_descriptor_set(
    SwapUniformBuffer<window::WindowDimension<float>> &uniform_buffers) {
    return DescriptorSetBuilder(m_descriptor_set_layout, m_descriptor_pool,
                                MAX_FRAMES_IN_FLIGHT)
        .set_uniform_buffer(0, uniform_buffers.get_buffer_references())
        .build(m_ctx);
}

Pipeline UIPipeline::create_pipeline(SwapChainManager &swap_chain_manager) {
    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code = resoure_manager.get_resource<ShaderResource>("UiVertex");
    auto frag_shader_code = resoure_manager.get_resource<ShaderResource>("UiFragment");

    ShaderModule vertex_shader = ShaderModule(m_ctx, *vert_shader_code);
    ShaderModule fragment_shader = ShaderModule(m_ctx, *frag_shader_code);

    VkPushConstantRange push_constants_ui_element{};
    push_constants_ui_element.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constants_ui_element.offset = 0;
    push_constants_ui_element.size = sizeof(ElementProperties);

    const std::vector<VkPushConstantRange> push_constant_ranges = {
        push_constants_ui_element};

    Pipeline pipeline = Pipeline(m_ctx, m_descriptor_set_layout, push_constant_ranges,
                                 vertex_shader.shader_module,
                                 fragment_shader.shader_module, swap_chain_manager);

    return pipeline;
}
