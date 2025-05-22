#include "UIPipeline.h"
#include "render_engine/DescriptorSetLayoutBuilder.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/resources/ResourceManager.h"
#include "render_engine/resources/shaders/ShaderResource.h"
#include "vulkan/vulkan_core.h"

UIPipeline::UIPipeline(std::shared_ptr<CoreGraphicsContext> ctx,
                       SwapChainManager &swap_chain_manager)
    : m_ctx(ctx), m_descriptor_set_layout(create_descriptor_set_layout()),
      m_pipeline(create_pipeline(swap_chain_manager)) {}

UIPipeline::~UIPipeline() {
    vkDestroyDescriptorSetLayout(m_ctx->device, m_descriptor_set_layout, nullptr);
}

void record_draw_commands(const VkCommandBuffer &command_buffer,
                          const VkPipeline &graphics_pipeline,
                          const VkPipelineLayout &pipeline_layout) {
    const VkDeviceSize vertex_buffers_offset = 0;
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);
    vkCmdDraw(command_buffer, 6, 1, 0, 0);
}

void UIPipeline::render(const VkCommandBuffer &command_buffer) {
    record_draw_commands(command_buffer, m_pipeline.m_pipeline,
                         m_pipeline.m_pipeline_layout);
}

VkDescriptorSetLayout UIPipeline::create_descriptor_set_layout() {
    return DescriptorSetLayoutBuilder().build(m_ctx.get());
}

Pipeline UIPipeline::create_pipeline(SwapChainManager &swap_chain_manager) {

    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code = resoure_manager.get_resource<ShaderResource>("UIVertex");
    auto frag_shader_code = resoure_manager.get_resource<ShaderResource>("UIFragment");

    VkShaderModule vert_shader_module = createShaderModule(
        m_ctx->device, vert_shader_code->bytes(), vert_shader_code->length());

    VkShaderModule frag_shader_module = createShaderModule(
        m_ctx->device, frag_shader_code->bytes(), frag_shader_code->length());

    Pipeline pipeline = Pipeline(m_ctx, m_descriptor_set_layout, vert_shader_module,
                                 frag_shader_module, swap_chain_manager);

    vkDestroyShaderModule(m_ctx->device, vert_shader_module, nullptr);
    vkDestroyShaderModule(m_ctx->device, frag_shader_module, nullptr);
    return pipeline;
}
