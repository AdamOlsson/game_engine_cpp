#pragma once

#include "DebugMessenger.h"
#include "Device.h"
#include "Instance.h"
#include "Surface.h"
#include "vulkan/vulkan_core.h"
#include "window/Window.h"

namespace vulkan::context {
struct DeviceQueues {
    VkQueue graphics_queue;
    VkQueue present_queue;
};

class GraphicsContext {
  private:
    bool m_enable_validation_layers;

    const std::vector<const char *> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};

  public:
    window::Window *window;
    Instance instance;
    Surface surface;
    device::PhysicalDevice physical_device;
    device::LogicalDevice logical_device;

    std::optional<DebugMessenger> m_debug_messenger;

    GraphicsContext(window::Window *window);
    ~GraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};

} // namespace vulkan::context
