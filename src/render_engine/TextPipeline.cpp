#include "render_engine/TextPipeline.h"
#include "CoreGraphicsContext.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/GeometryPipeline.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/resources/ResourceManager.h"
#include "vulkan/vulkan_core.h"
#include <cstring>
#include <memory>
#include <vector>

TextPipeline::TextPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                           SwapChainManager &swap_chain_manager,
                           std::vector<UniformBuffer> &uniform_buffers,
                           VkDescriptorSetLayout &descriptor_set_layout, Sampler &sampler,
                           Texture &texture)
    : m_ctx(ctx), m_pipeline(create_pipeline(descriptor_set_layout, swap_chain_manager)),
      m_descriptor_pool(DescriptorPool(m_ctx, MAX_FRAMES_IN_FLIGHT)) {

    auto descriptor_set_builder = DescriptorSetBuilder()
                                      .set_descriptor_set_layout(descriptor_set_layout)
                                      .set_descriptor_pool(&m_descriptor_pool)
                                      .set_capacity(MAX_FRAMES_IN_FLIGHT)
                                      .set_uniform_buffers(&uniform_buffers)
                                      .set_texture(&texture)
                                      .set_sampler(&sampler);

    auto descriptor_set = descriptor_set_builder.build(m_ctx);
    geometry = std::make_unique<Geometry::Rectangle>(ctx, swap_chain_manager,
                                                     std::move(descriptor_set));
}

TextPipeline::~TextPipeline() {}

Pipeline TextPipeline::create_pipeline(VkDescriptorSetLayout &descriptor_set_layout,
                                       SwapChainManager &swap_chain_manager) {
    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code = resoure_manager.get_resource<ShaderResource>("Vert");
    auto frag_shader_code = resoure_manager.get_resource<ShaderResource>("TextFragment");

    VkShaderModule vert_shader_module = createShaderModule(
        m_ctx->device, vert_shader_code->bytes(), vert_shader_code->length());

    VkShaderModule frag_shader_module = createShaderModule(
        m_ctx->device, frag_shader_code->bytes(), frag_shader_code->length());

    Pipeline pipeline = Pipeline(m_ctx, descriptor_set_layout, vert_shader_module,
                                 frag_shader_module, swap_chain_manager);

    vkDestroyShaderModule(m_ctx->device, vert_shader_module, nullptr);
    vkDestroyShaderModule(m_ctx->device, frag_shader_module, nullptr);
    return pipeline;
}

void TextPipeline::render_text(const VkCommandBuffer &command_buffer,
                               std::vector<StorageBufferObject> &&instance_data) {
    geometry->update_instance_buffer(
        std::forward<std::vector<StorageBufferObject>>(instance_data));
    const auto num_instances = instance_data.size();
    if (num_instances > 0) {
        geometry->record_draw_command(command_buffer, m_pipeline.m_pipeline,
                                      m_pipeline.m_pipeline_layout, num_instances);
    }
}
