#pragma once

#include "game_engine_sdk/render_engine/vulkan/DebugMessenger.h"
#include "game_engine_sdk/render_engine/vulkan/Device.h"
#include "game_engine_sdk/render_engine/vulkan/Instance.h"
#include "game_engine_sdk/render_engine/vulkan/Surface.h"
#include "game_engine_sdk/render_engine/window/Window.h"
#include "vulkan/vulkan_core.h"

namespace vulkan {
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
    vulkan::Instance instance;
    vulkan::Surface surface;
    vulkan::device::PhysicalDevice physical_device;
    vulkan::device::LogicalDevice logical_device;

    std::optional<vulkan::DebugMessenger> m_debug_messenger;

    GraphicsContext(window::Window *window);
    ~GraphicsContext();

    void wait_idle();

    DeviceQueues get_device_queues();
};

} // namespace vulkan
