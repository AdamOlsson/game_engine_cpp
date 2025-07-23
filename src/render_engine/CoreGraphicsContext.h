#pragma once

#include "render_engine/window/Window.h"
#include "vulkan/vulkan_core.h"
#include <vector>

struct DeviceQueues {
    VkQueue graphics_queue;
    VkQueue present_queue;
};

class CoreGraphicsContext {

    bool m_enable_validation_layers;
    VkInstance m_instance;
    std::optional<VkDebugUtilsMessengerEXT> m_debug_messenger;

    VkInstance create_instance();
    VkSurfaceKHR create_surface(GLFWwindow &window);
    VkPhysicalDevice pick_physical_device(VkInstance &instance, VkSurfaceKHR &surface);
    VkDevice create_logical_device(const std::vector<const char *> &deviceExtensions);
    void print_enabled_extensions();
    std::optional<VkDebugUtilsMessengerEXT> setup_debug_messenger();
    bool check_validation_layer_support();

    bool check_device_extension_support(const VkPhysicalDevice &physicalDevice);
    std::vector<const char *> get_required_extensions();

    void
    populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    VkResult create_debug_utils_messenger_ext(
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        VkDebugUtilsMessengerEXT *pDebugMessenger);

    void destroy_debug_messenger_ext();

    bool is_device_suitable(const VkPhysicalDevice &physicalDevice);

  public:
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;

    CoreGraphicsContext(const window::Window &window);
    ~CoreGraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};
