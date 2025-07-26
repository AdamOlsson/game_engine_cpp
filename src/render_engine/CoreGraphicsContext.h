#pragma once

#include "render_engine/Instance.h"
#include "render_engine/window/Window.h"
#include "vulkan/vulkan_core.h"
#include <vector>

struct DeviceQueues {
    VkQueue graphics_queue;
    VkQueue present_queue;
};

class CoreGraphicsContext {

    bool m_enable_validation_layers;

    VkSurfaceKHR create_surface(GLFWwindow &window);
    VkPhysicalDevice pick_physical_device(VkInstance &instance);
    VkDevice create_logical_device(const std::vector<const char *> &deviceExtensions);
    void print_enabled_extensions();
    std::optional<VkDebugUtilsMessengerEXT> setup_debug_messenger();

    bool check_device_extension_support(const VkPhysicalDevice &physicalDevice);
    std::vector<const char *> get_required_extensions();

    VkResult create_debug_utils_messenger_ext(
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        VkDebugUtilsMessengerEXT *pDebugMessenger);

    void destroy_debug_messenger_ext();

    bool is_device_suitable(const VkPhysicalDevice &physicalDevice);

  public:
    window::Window *window;
    Instance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;

    std::optional<VkDebugUtilsMessengerEXT> m_debug_messenger;

    CoreGraphicsContext(window::Window *window);
    ~CoreGraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};
