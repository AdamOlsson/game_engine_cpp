#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/DescriptorPool.h"
#include "render_engine/Pipeline.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/SwapChainManager.h"
#include "render_engine/Texture.h"
#include "render_engine/Window.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int MAX_FRAMES_IN_FLIGHT = 2;

VKAPI_ATTR inline VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

std::vector<char> readFile(const std::string filename);
VkShaderModule createShaderModule(const VkDevice &device, const uint8_t *data,
                                  const size_t len);
VkDescriptorPool createDescriptorPool(VkDevice &device, const int capacity);

class GeometryPipeline {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

    Pipeline m_pipeline;
    DescriptorPool m_descriptor_pool;

    SwapStorageBuffer<StorageBufferObject> m_circle_instance_buffers;
    SwapStorageBuffer<StorageBufferObject> m_triangle_instance_buffers;
    SwapStorageBuffer<StorageBufferObject> m_rectangle_instance_buffers;
    SwapStorageBuffer<StorageBufferObject> m_hexagon_instance_buffers;

    std::unique_ptr<Geometry::Circle> circle_geometry;
    std::unique_ptr<Geometry::Triangle> triangle_geometry;
    std::unique_ptr<Geometry::Rectangle> rectangle_geometry;
    std::unique_ptr<Geometry::Hexagon> hexagon_geometry;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool framebufferResized = false;

    Pipeline create_pipeline(VkDescriptorSetLayout &descriptorSetLayout,
                             SwapChainManager &swap_chain_manager);
    bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);

    void updateUniformBuffer(uint32_t currentImage);

  public:
    GeometryPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                     SwapChainManager &swap_chain_manager,
                     std::vector<UniformBuffer> &uniform_buffers,
                     VkDescriptorSetLayout &descriptor_set_layout, Sampler &sampler,
                     Texture &texture);
    ~GeometryPipeline();

    StorageBuffer<StorageBufferObject> &get_circle_instance_buffer();
    StorageBuffer<StorageBufferObject> &get_triangle_instance_buffer();
    StorageBuffer<StorageBufferObject> &get_rectangle_instance_buffer();
    StorageBuffer<StorageBufferObject> &get_hexagon_instance_buffer();

    // TODO: These render function should merge into one generic call
    void render_circles(const VkCommandBuffer &command_buffer);
    void render_triangles(const VkCommandBuffer &command_buffer);
    void render_rectangles(const VkCommandBuffer &command_buffer);
    void render_hexagons(const VkCommandBuffer &command_buffer);
};
