#include "CoreGraphicsContext.h"
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
      physical_device(pick_physical_device(instance.instance)),
      device(create_logical_device(device_extensions)) {

    if (m_enable_validation_layers) {
        m_debug_messenger = validation_layers::messenger::DebugMessenger(instance);
    }
}

CoreGraphicsContext::~CoreGraphicsContext() { vkDestroyDevice(device, nullptr); }

void CoreGraphicsContext::wait_idle() { vkDeviceWaitIdle(device); }

DeviceQueues CoreGraphicsContext::get_device_queues() {
    QueueFamilyIndices indices_ = findQueueFamilies(physical_device, surface.surface);
    VkQueue graphics_queue;
    VkQueue present_queue;
    uint32_t index = 0;
    vkGetDeviceQueue(device, indices_.graphicsFamily.value(), index, &graphics_queue);
    vkGetDeviceQueue(device, indices_.presentFamily.value(), index, &present_queue);
    return DeviceQueues{.graphics_queue = graphics_queue, .present_queue = present_queue};
}

bool CoreGraphicsContext::check_device_extension_support(
    const VkPhysicalDevice &physicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                         nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(device_extensions.begin(),
                                             device_extensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool CoreGraphicsContext::is_device_suitable(const VkPhysicalDevice &physicalDevice) {
    // NOTE: For mor info check:
    // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
    /*VkPhysicalDeviceProperties deviceProperties;*/
    /*vkGetPhysicalDeviceProperties(device, &deviceProperties);*/

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface.surface);
    bool extensionsSupported = check_device_extension_support(physicalDevice);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport =
            querySwapChainSupport(physicalDevice, surface.surface);
        swapChainAdequate =
            !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
           deviceFeatures.samplerAnisotropy;
}

VkPhysicalDevice CoreGraphicsContext::pick_physical_device(VkInstance &m_instance) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

    if (device_count == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    for (const VkPhysicalDevice &device : devices) {
        if (is_device_suitable(device)) {
            physical_device = device;
            break;
        }
    }

    if (physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    return physical_device;
}

VkDevice CoreGraphicsContext::create_logical_device(
    const std::vector<const char *> &deviceExtensions) {
    QueueFamilyIndices indices = findQueueFamilies(physical_device, surface.surface);

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
    if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    return device;
}
