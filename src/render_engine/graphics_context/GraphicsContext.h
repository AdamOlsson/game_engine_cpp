#pragma once

#include "Device.h"
#include "Instance.h"
#include "Surface.h"
#include "render_engine/window/Window.h"
#include "validation_layers.h"
#include "vulkan/vulkan_core.h"

namespace graphics_context {
struct DeviceQueues {
    VkQueue graphics_queue;
    VkQueue present_queue;
};

class GraphicsContext {
    bool m_enable_validation_layers;

  public:
    window::Window *window;
    Instance instance;
    Surface surface;
    device::PhysicalDevice physical_device;
    device::LogicalDevice logical_device;

    std::optional<validation_layers::messenger::DebugMessenger> m_debug_messenger;

    GraphicsContext(window::Window *window);
    ~GraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};

} // namespace graphics_context
