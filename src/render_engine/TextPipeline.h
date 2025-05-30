#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/Pipeline.h"
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
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    Pipeline m_pipeline;
    DescriptorPool m_descriptor_pool;

    size_t m_buffer_idx;
    std::vector<StorageBuffer> m_instance_buffers;

    std::unique_ptr<Geometry::Rectangle> geometry;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool framebufferResized = false;
    Pipeline create_pipeline(VkDescriptorSetLayout &descriptor_set_layout,
                             SwapChainManager &swap_chain_manager);

    bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);

    void updateUniformBuffer(uint32_t currentImage);

  public:
    TextPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                 SwapChainManager &swap_chain,
                 std::vector<UniformBuffer> &uniform_buffers,
                 VkDescriptorSetLayout &descriptor_set_layout, Sampler &sampler,
                 Texture &texture);
    ~TextPipeline();

    void render_text(const VkCommandBuffer &command_buffer,
                     std::vector<StorageBufferObject> &&instance_data);
};
