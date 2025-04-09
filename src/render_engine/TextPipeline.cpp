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
#include "util.h"
#include "vulkan/vulkan_core.h"
#include <cstring>
#include <memory>
#include <vector>

static std::vector<char> readFile(const std::string filename);

VkShaderModule createShaderModule(const VkDevice &device, const std::vector<char> &code);

VkPipeline createGraphicsPipeline(const VkDevice &device,
                                  const std::string vertex_shader_path,
                                  const std::string fragment_shader_path,
                                  VkDescriptorSetLayout &descriptorSetLayout,
                                  VkPipelineLayout &pipelineLayout,
                                  VkRenderPass &renderPass, SwapChain &swap_chain);

TextPipeline::TextPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                           CommandHandler &command_handler, SwapChain &swap_chain,
                           VkRenderPass &render_pass,
                           std::vector<UniformBuffer> &uniform_buffers,
                           VkDescriptorSetLayout &descriptor_set_layout, Sampler &sampler,
                           Texture &texture)
    : ctx(ctx) {

    auto [graphicsQueue, presentQueue] = ctx->get_device_queues();

    // TODO: Not needed to include the bytes of the shader twice. Move shader module
    // creation to parent
    graphics_pipeline = createGraphicsPipeline(
        ctx->device, "src/render_engine/shaders/vert.spv",
        "src/render_engine/shaders/text_fragment.spv", descriptor_set_layout,
        pipeline_layout, render_pass, swap_chain);

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
