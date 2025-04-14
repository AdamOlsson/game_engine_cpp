#pragma once

#include "render_engine/CommandHandler.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/Sampler.h"
#include "render_engine/SwapChain.h"
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
VkPipeline createGraphicsPipeline(const VkDevice &device,
                                  const VkShaderModule vertShaderModule,
                                  const VkShaderModule fragShaderModule,
                                  VkDescriptorSetLayout &descriptorSetLayout,
                                  VkPipelineLayout &pipelineLayout,
                                  VkRenderPass &renderPass, SwapChain &swapChain);

class GraphicsPipeline {
  public:
    GraphicsPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx,
                     CommandHandler &command_handler, SwapChain &swap_chain,
                     VkRenderPass &render_pass,
                     std::vector<UniformBuffer> &uniform_buffers,
                     VkDescriptorSetLayout &geometry_descriptor_set_layout,
                     Sampler &sampler, Texture &texture);
    ~GraphicsPipeline();

    // TODO: These render function should merge into one generic call
    void render_circles(const VkCommandBuffer &command_buffer,
                        std::vector<StorageBufferObject> &&circle_instance_data);
    void render_triangles(const VkCommandBuffer &command_buffer,
                          std::vector<StorageBufferObject> &&triangle_instance_data);
    void render_rectangles(const VkCommandBuffer &command_buffer,
                           std::vector<StorageBufferObject> &&rectangle_instance_data);
    void render_hexagons(const VkCommandBuffer &command_buffer,
                         std::vector<StorageBufferObject> &&hexagon_instance_data);

  private:
    std::shared_ptr<CoreGraphicsContext> ctx;

    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
    VkDescriptorPool descriptor_pool;

    std::unique_ptr<Geometry::Circle> circle_geometry;
    std::unique_ptr<Geometry::Triangle> triangle_geometry;
    std::unique_ptr<Geometry::Rectangle> rectangle_geometry;
    std::unique_ptr<Geometry::Hexagon> hexagon_geometry;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool framebufferResized = false;

    bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);

    void updateUniformBuffer(uint32_t currentImage);
};
