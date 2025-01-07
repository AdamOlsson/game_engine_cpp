#pragma once

#include "render_engine/Window.h"
#include "render_engine/buffers/IndexBuffer.h"
#include "render_engine/buffers/StorageBuffer.h"
#include "render_engine/buffers/UniformBuffer.h"
#include "render_engine/buffers/VertexBuffer.h"
#include "render_engine/shapes/Triangle.h"
#include "render_engine/shapes/Vertex.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

const int MAX_FRAMES_IN_FLIGHT = 2;

struct Device {
    VkDevice device;
    Device(VkDevice device) : device(device) {}
    ~Device() {
        if (device == nullptr) {
            return;
        }
        vkDestroyDevice(device, nullptr);
    }
};

const std::vector<Vertex> vertices = Triangle::vertices;
const std::vector<uint16_t> indices = Triangle::indices;

VKAPI_ATTR inline VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class GraphicsContext {
  public:
    GraphicsContext(Window &window);
    ~GraphicsContext();

    void waitIdle();
    void render(Window &window, const std::vector<StorageBufferObject> &ssbo);

  private:
    uint32_t currentFrame = 0;
    bool enableValidationLayers;
    VkInstance instance;
    std::optional<VkDebugUtilsMessengerEXT> debugMessenger;

    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    VkRenderPass renderPass;

    VkDescriptorSetLayout bufferDescriptorSetLayout;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;

    std::vector<std::unique_ptr<UniformBuffer>> uniformBuffers;
    std::vector<std::unique_ptr<StorageBuffer>> storageBuffers;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool framebufferResized = false;

    void recreateSwapChain(VkDevice &device, GLFWwindow &window);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    bool checkValidationLayerSupport();

    std::vector<const char *> getRequiredExtensions(bool enableValidationLayers);

    VkInstance createInstance(bool enableValidationLayers);

    std::optional<VkDebugUtilsMessengerEXT>
    setupDebugMessenger(VkInstance &instance, bool enableValidationLayers);

    VkResult
    CreateDebugUtilsMessengerEXT(VkInstance instance,
                                 const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                 const VkAllocationCallbacks *pAllocator,
                                 VkDebugUtilsMessengerEXT *pDebugMessenger);

    VkPhysicalDevice pickPhysicalDevice(VkInstance &instance);
    bool isDeviceSuitable(const VkPhysicalDevice &physicalDevice);

    VkDevice createLogicalDevice(VkPhysicalDevice &physicalDevice,
                                 const std::vector<const char *> &deviceExtensions);

    std::vector<VkImageView> createImageViews(VkDevice &device,
                                              std::vector<VkImage> &swapChainImages);

    VkRenderPass createRenderPass(VkDevice &device, VkFormat &swapChainImageFormat);

    VkSurfaceFormatKHR
    chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR
    chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(GLFWwindow &window,
                                const VkSurfaceCapabilitiesKHR &capabilities);

    std::tuple<VkSwapchainKHR, std::vector<VkImage>, VkFormat, VkExtent2D>
    createSwapChain(VkPhysicalDevice &physicalDevice, VkDevice &device,
                    GLFWwindow &window);

    VkSurfaceKHR createSurface(VkInstance *instance, GLFWwindow &window);

    bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);

    SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &physicalDevice);

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device);

    VkPipeline createGraphicsPipeline(VkDevice &device,
                                      const std::string vertex_shader_path,
                                      const std::string fragment_shader_path,
                                      VkDescriptorSetLayout &descriptorSetLayout);

    std::vector<VkCommandBuffer> createCommandBuffers(VkDevice &device,
                                                      const int capacity);

    std::vector<VkFramebuffer>
    createFramebuffers(VkDevice &device, std::vector<VkImageView> &swapChainImageViews);

    VkCommandPool createCommandPool(VkPhysicalDevice &physicalDevice, VkDevice &device);

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                             uint32_t currentFrame, const VkBuffer vertexBuffer,
                             const std::vector<uint16_t> indices);

    std::tuple<std::vector<VkSemaphore>, std::vector<VkSemaphore>, std::vector<VkFence>>
    createSyncObjects(VkDevice &device, const int capacity);

    void updateUniformBuffer(uint32_t currentImage);

    std::vector<VkDescriptorSet>
    createDescriptorSets(VkDevice &device, VkDescriptorSetLayout &descriptorSetLayout,
                         const int capacity);

    VkDescriptorPool createDescriptorPool(VkDevice &device, const int capacity);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);

    void cleanupSwapChain(VkDevice &device);
};
