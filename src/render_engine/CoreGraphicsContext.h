#pragma once

#include "render_engine/Instance.h"
#include "render_engine/PhysicalDevice.h"
#include "render_engine/Surface.h"
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

    VkDevice create_logical_device(const std::vector<const char *> &deviceExtensions);

  public:
    window::Window *window;
    Instance instance;
    Surface surface;
    PhysicalDevice physical_device;
    VkDevice device;

    std::optional<validation_layers::messenger::DebugMessenger> m_debug_messenger;

    CoreGraphicsContext(window::Window *window);
    ~CoreGraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};
