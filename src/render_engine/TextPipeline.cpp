#include "render_engine/TextPipeline.h"
#include "CoreGraphicsContext.h"
#include "render_engine/CommandHandler.h"
#include "render_engine/DescriptorSet.h"
#include "render_engine/GraphicsPipeline.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/Texture.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/resources/ResourceManager.h"
#include "util.h"
#include "vulkan/vulkan_core.h"
#include <cstring>
#include <memory>
#include <vector>

TextPipeline::TextPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                           CommandHandler &command_handler, SwapChain &swap_chain,
                           VkRenderPass &render_pass,
                           std::vector<UniformBuffer> &uniform_buffers,
                           VkDescriptorSetLayout &descriptor_set_layout, Sampler &sampler,
                           Texture &texture)
    : ctx(ctx) {

    auto [graphicsQueue, presentQueue] = ctx->get_device_queues();

    // TODO: Script that automatically includes compiled textures into ResourceManager
    auto &resoure_manager = ResourceManager::get_instance();
    auto vert_shader_code = resoure_manager.get_resource<ShaderResource>("Vert");
    auto frag_shader_code = resoure_manager.get_resource<ShaderResource>("TextFragment");

    VkShaderModule vert_shader_module = createShaderModule(
        ctx->device, vert_shader_code->bytes(), vert_shader_code->length());

    VkShaderModule frag_shader_module = createShaderModule(
        ctx->device, frag_shader_code->bytes(), frag_shader_code->length());

    graphics_pipeline = createGraphicsPipeline(ctx->device, vert_shader_module,
                                               frag_shader_module, descriptor_set_layout,
                                               pipeline_layout, render_pass, swap_chain);

    vkDestroyShaderModule(ctx->device, vert_shader_module, nullptr);
    vkDestroyShaderModule(ctx->device, frag_shader_module, nullptr);

    VkCommandPool command_pool = command_handler.pool();

    descriptor_pool = createDescriptorPool(ctx->device, MAX_FRAMES_IN_FLIGHT);
    auto descriptor_set =
        DescriptorSet(ctx, descriptor_set_layout, descriptor_pool, MAX_FRAMES_IN_FLIGHT,
                      &uniform_buffers, &texture, &sampler);
    geometry = std::make_unique<Geometry::Rectangle>(ctx, command_pool, graphicsQueue,
                                                     std::move(descriptor_set));
}

TextPipeline::~TextPipeline() {
    // Order of these functions matter
    vkDestroyDescriptorPool(ctx->device, descriptor_pool, nullptr);
    vkDestroyPipeline(ctx->device, graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(ctx->device, pipeline_layout, nullptr);
}

void TextPipeline::render_text(const VkCommandBuffer &command_buffer,
                               std::vector<StorageBufferObject> &&instance_data) {
    geometry->update_instance_buffer(
        std::forward<std::vector<StorageBufferObject>>(instance_data));
    const auto num_instances = instance_data.size();
    if (num_instances > 0) {
        geometry->record_draw_command(command_buffer, graphics_pipeline, pipeline_layout,
                                      num_instances);
    }
}
