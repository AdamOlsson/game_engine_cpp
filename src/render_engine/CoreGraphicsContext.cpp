#include "CoreGraphicsContext.h"
#include "render_engine/PhysicalDevice.h"
#include "render_engine/validation_layers.h"
#include "render_engine/window/Window.h"
#include "util.h"
#include "vulkan/vulkan_beta.h"
#include "vulkan/vulkan_core.h"
#include <set>
#include <stdexcept>

namespace {

const std::vector<const char *> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

} // namespace

CoreGraphicsContext::CoreGraphicsContext(window::Window *window)
    : m_enable_validation_layers(true), window(window),
      instance(Instance(m_enable_validation_layers)), surface(Surface(instance, *window)),
      physical_device(PhysicalDevice(instance, surface)),
      device(create_logical_device(device_extensions)) {

    if (m_enable_validation_layers) {
        m_debug_messenger = validation_layers::messenger::DebugMessenger(instance);
    }
}

CoreGraphicsContext::~CoreGraphicsContext() {
    if (device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
    }

    m_debug_messenger.reset();

    vkDestroyDevice(device, nullptr);
}

void CoreGraphicsContext::wait_idle() { vkDeviceWaitIdle(device); }

DeviceQueues CoreGraphicsContext::get_device_queues() {
    QueueFamilyIndices indices_ = findQueueFamilies(physical_device, surface);
    VkQueue graphics_queue;
    VkQueue present_queue;
    uint32_t index = 0;
    vkGetDeviceQueue(device, indices_.graphicsFamily.value(), index, &graphics_queue);
    vkGetDeviceQueue(device, indices_.presentFamily.value(), index, &present_queue);
    return DeviceQueues{.graphics_queue = graphics_queue, .present_queue = present_queue};
}

VkDevice CoreGraphicsContext::create_logical_device(
    const std::vector<const char *> &deviceExtensions) {
    QueueFamilyIndices indices = findQueueFamilies(physical_device, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    std::vector<const char *> extendedDeviceExtensions(deviceExtensions);
    if (m_enable_validation_layers) {
        extendedDeviceExtensions.emplace_back(
            VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME); // needed for macOS
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(extendedDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = extendedDeviceExtensions.data();

    if (m_enable_validation_layers) {
        // Deprecated on new versions of vulkan, set them here for backwards
        // compatability
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validation_layers::validation_layers.size());
        createInfo.ppEnabledLayerNames = validation_layers::validation_layers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkDevice device;
    if (vkCreateDevice(physical_device.physical_device, &createInfo, nullptr, &device) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    return device;
}
