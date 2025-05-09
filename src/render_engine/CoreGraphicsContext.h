#pragma once

#include "render_engine/Window.h"
#include "vulkan/vulkan_core.h"
#include <vector>

struct DeviceQueues {
    VkQueue graphics_queue;
    VkQueue present_queue;
};

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

class CoreGraphicsContext {
  public:
    bool enableValidationLayers;
    VkInstance instance;
    std::optional<VkDebugUtilsMessengerEXT> debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    CoreGraphicsContext(const bool enableValidationLayers, const Window &window);
    ~CoreGraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};

void populateDebugMessengerCreateInfo__(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
bool checkValidationLayerSupport();
std::vector<const char *> getRequiredExtensions(bool enableValidationLayers);
VkInstance createInstance(bool enableValidationLayers);
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
std::optional<VkDebugUtilsMessengerEXT> setupDebugMessenger(VkInstance &instance,
                                                            bool enableValidationLayers);
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator);
VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow &window);
VkPhysicalDevice pickPhysicalDevice(VkInstance &instance, VkSurfaceKHR &surface);
bool isDeviceSuitable(const VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface);
bool checkDeviceExtensionSupport(const VkPhysicalDevice &physicalDevice);
VkDevice createLogicalDevice(VkPhysicalDevice &physicalDevice, VkSurfaceKHR &surface,
                             const std::vector<const char *> &deviceExtensions,
                             bool enableValidationLayers);
void print_enabled_extensions(VkInstance &instance);
