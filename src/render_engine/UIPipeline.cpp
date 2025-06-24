#include "ui/UIPipeline.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/DescriptorSetLayoutBuilder.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include "vulkan/vulkan_core.h"

// TODO:
// 4. Implement text in UI elements

using namespace ui;

UIPipeline::UIPipeline(std::shared_ptr<CoreGraphicsContext> ctx,
                       SwapChainManager &swap_chain_manager,
                       std::vector<UniformBuffer> &uniform_buffers)
    : m_ctx(ctx),
      // TODO: Is it not better to merge all these to a single Descriptor class
      m_descriptor_set_layout(create_descriptor_set_layout()),
      m_descriptor_pool(DescriptorPool(m_ctx, MAX_FRAMES_IN_FLIGHT)),
      m_descriptor_set(create_descriptor_set(uniform_buffers)),
      m_pipeline(create_pipeline(swap_chain_manager)),
      m_vertex_buffer(m_ctx, {{0.0, 0.0, 0.0}}, swap_chain_manager) {}

UIPipeline::~UIPipeline() {
    vkDestroyDescriptorSetLayout(m_ctx->device, m_descriptor_set_layout, nullptr);
}

void UIPipeline::render(const VkCommandBuffer &command_buffer,
                        const ElementProperties &ui_element) {
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_pipeline.m_pipeline);

    vkCmdPushConstants(command_buffer, m_pipeline.m_pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(ElementProperties), &ui_element);

    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer.buffer,
                           &vertex_buffers_offset);

    auto descriptor = m_descriptor_set.get(); // TODO: This should be not be needed.
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_pipeline.m_pipeline_layout, 0, 1, &descriptor, 0, nullptr);

    vkCmdDraw(command_buffer, 6, 1, 0, 0);
}

VkDescriptorSetLayout UIPipeline::create_descriptor_set_layout() {
    return DescriptorSetLayoutBuilder()
        .add(UniformBuffer::create_descriptor_set_layout_binding(0))
        .build(m_ctx.get());
}

DescriptorSet
UIPipeline::create_descriptor_set(std::vector<UniformBuffer> &uniform_buffers) {
    return DescriptorSetBuilder(m_descriptor_set_layout, m_descriptor_pool,
                                MAX_FRAMES_IN_FLIGHT)
        .set_uniform_buffers(0, uniform_buffers)
        .build(m_ctx);
}

Pipeline UIPipeline::create_pipeline(SwapChainManager &swap_chain_manager) {
    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code = resoure_manager.get_resource<ShaderResource>("UiVertex");
    auto frag_shader_code = resoure_manager.get_resource<ShaderResource>("UiFragment");

    VkShaderModule vert_shader_module = createShaderModule(
        m_ctx->device, vert_shader_code->bytes(), vert_shader_code->length());

    VkShaderModule frag_shader_module = createShaderModule(
        m_ctx->device, frag_shader_code->bytes(), frag_shader_code->length());

    VkPushConstantRange push_constants_ui_element{};
    push_constants_ui_element.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constants_ui_element.offset = 0;
    push_constants_ui_element.size = sizeof(ElementProperties);

    const std::vector<VkPushConstantRange> push_constant_ranges = {
        push_constants_ui_element};

    Pipeline pipeline =
        Pipeline(m_ctx, m_descriptor_set_layout, push_constant_ranges, vert_shader_module,
                 frag_shader_module, swap_chain_manager);

    vkDestroyShaderModule(m_ctx->device, vert_shader_module, nullptr);
    vkDestroyShaderModule(m_ctx->device, frag_shader_module, nullptr);
    return pipeline;
}
