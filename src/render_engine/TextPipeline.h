#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/Texture.h"
#include "render_engine/Window.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TextPipeline {
  public:
    TextPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                 SwapChainManager &swap_chain,
                 std::vector<UniformBuffer> &uniform_buffers,
                 VkDescriptorSetLayout &descriptor_set_layout, Sampler &sampler,
                 Texture &texture);
    ~TextPipeline();

    void render_text(const VkCommandBuffer &command_buffer,
                     std::vector<StorageBufferObject> &&instance_data);

  private:
    std::shared_ptr<CoreGraphicsContext> ctx;

    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
    VkDescriptorPool descriptor_pool;
    std::unique_ptr<Geometry::Rectangle> geometry;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool framebufferResized = false;

    bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);

    void updateUniformBuffer(uint32_t currentImage);
};
