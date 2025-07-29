#pragma once

#include "render_engine/Instance.h"
#include "render_engine/PhysicalDevice.h"
#include "render_engine/Surface.h"
#include "render_engine/validation_layers.h"
#include "render_engine/window/Window.h"
#include "vulkan/vulkan_core.h"

struct DeviceQueues {
    VkQueue graphics_queue;
    VkQueue present_queue;
};

class CoreGraphicsContext {
    bool m_enable_validation_layers;

  public:
    window::Window *window;
    Instance instance;
    Surface surface;
    device::PhysicalDevice physical_device;
    device::LogicalDevice logical_device;

    std::optional<validation_layers::messenger::DebugMessenger> m_debug_messenger;

    CoreGraphicsContext(window::Window *window);
    ~CoreGraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};
