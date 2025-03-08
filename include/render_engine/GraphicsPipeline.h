#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/RenderableGeometry.h"
#include "render_engine/SwapChain.h"
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

class GraphicsPipeline {
  public:
    GraphicsPipeline(Window &window, std::shared_ptr<CoreGraphicsContext> ctx);
    ~GraphicsPipeline();

    void render(Window &window, std::vector<StorageBufferObject> &&circle_instance_data,
                std::vector<StorageBufferObject> &&triangle_instance_data,
                std::vector<StorageBufferObject> &&rectangle_instance_data,
                std::vector<StorageBufferObject> &&hexagon_instance_data,
                std::vector<StorageBufferObject> &&arrow_instance_data,
                std::vector<StorageBufferObject> &&line_instance_data);

  private:
    uint32_t currentFrame = 0;

    std::shared_ptr<CoreGraphicsContext> ctx;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    SwapChain swapChain;

    VkRenderPass renderPass;

    VkDescriptorSetLayout bufferDescriptorSetLayout;

    VkCommandPool commandPool;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    std::vector<std::unique_ptr<UniformBuffer>> uniformBuffers;

    VkPipelineLayout geometryPipelineLayout;
    VkPipeline geometryGraphicsPipeline;
    VkDescriptorPool geometryDescriptorPool;

    std::unique_ptr<Geometry::Circle> circle_geometry;
    std::vector<VkDescriptorSet> circle_descriptor_sets;

    std::unique_ptr<Geometry::Triangle> triangle_geometry;
    std::vector<VkDescriptorSet> triangle_descriptor_sets;

    std::unique_ptr<Geometry::Rectangle> rectangle_geometry;
    std::vector<VkDescriptorSet> rectangle_descriptor_sets;

    std::unique_ptr<Geometry::Hexagon> hexagon_geometry;
    std::vector<VkDescriptorSet> hexagon_descriptor_sets;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool framebufferResized = false;

    void recreateSwapChain(VkPhysicalDevice &physicalDevice, VkDevice &device,
                           VkSurfaceKHR &surface, GLFWwindow &window);

    std::vector<VkImageView> createImageViews(VkDevice &device,
                                              std::vector<VkImage> &swapChainImages);

    std::tuple<VkSwapchainKHR, std::vector<VkImage>, VkFormat, VkExtent2D>
    createSwapChain(VkPhysicalDevice &physicalDevice, VkDevice &device,
                    GLFWwindow &window);

    bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);

    std::vector<VkCommandBuffer> createCommandBuffers(VkDevice &device,
                                                      const int capacity);

    std::vector<VkFramebuffer>
    createFramebuffers(VkDevice &device, std::vector<VkImageView> &swapChainImageViews,
                       VkRenderPass &renderPass, VkExtent2D &swapChainExtent);

    VkCommandPool createCommandPool(VkPhysicalDevice &physicalDevice, VkDevice &device,
                                    VkSurfaceKHR &surface);

    void updateUniformBuffer(uint32_t currentImage);

    VkDescriptorPool createDescriptorPool(VkDevice &device, const int capacity);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);

    /*void cleanupSwapChain(VkDevice &device, SwapChain &);*/
};
