#include "CoreGraphicsContext.h"
#include "render_engine/validation_layers.h"
#include "util.h"
#include "vulkan/vulkan_core.h"

// CONTINUE: Find function like get_device_queues() below and integrate them with their
// specific class (i.e Instance, Surface, etc)
CoreGraphicsContext::CoreGraphicsContext(window::Window *window)
    : m_enable_validation_layers(true), window(window),
      instance(Instance(m_enable_validation_layers)), surface(Surface(&instance, window)),
      physical_device(device::PhysicalDevice(instance, surface)),
      logical_device(
          device::LogicalDevice(m_enable_validation_layers, surface, physical_device)) {

    if (m_enable_validation_layers) {
        m_debug_messenger = validation_layers::messenger::DebugMessenger(&instance);
    }
}

CoreGraphicsContext::~CoreGraphicsContext() {
    if (logical_device != VK_NULL_HANDLE) {
        logical_device.wait_idle();
    }

    m_debug_messenger.reset();
}

void CoreGraphicsContext::wait_idle() { logical_device.wait_idle(); }

DeviceQueues CoreGraphicsContext::get_device_queues() {
    QueueFamilyIndices indices_ = findQueueFamilies(physical_device, surface);
    VkQueue graphics_queue;
    VkQueue present_queue;
    uint32_t index = 0;
    vkGetDeviceQueue(logical_device, indices_.graphicsFamily.value(), index,
                     &graphics_queue);
    vkGetDeviceQueue(logical_device, indices_.presentFamily.value(), index,
                     &present_queue);
    return DeviceQueues{.graphics_queue = graphics_queue, .present_queue = present_queue};
}
