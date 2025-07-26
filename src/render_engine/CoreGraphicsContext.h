#pragma once

#include "render_engine/Instance.h"
#include "render_engine/validation_layers.h"
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

    bool check_device_extension_support(const VkPhysicalDevice &physicalDevice);

    bool is_device_suitable(const VkPhysicalDevice &physicalDevice);

  public:
    window::Window *window;
    Instance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;

    std::optional<validation_layers::messenger::DebugMessenger> m_debug_messenger;

    CoreGraphicsContext(window::Window *window);
    ~CoreGraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};
