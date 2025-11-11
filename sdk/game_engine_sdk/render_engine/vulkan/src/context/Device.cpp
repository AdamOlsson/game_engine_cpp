#include "vulkan/context/Device.h"
#include "vulkan/vulkan_beta.h"
#include "vulkan/vulkan_core.h"
#include <set>

namespace {

const std::vector<const char *> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
};

}

vulkan::context::device::PhysicalDevice::PhysicalDevice(const Instance &instance,
                                                        const Surface &surface)
    : m_physical_device(pick_physical_device(instance, surface)) {}

VkPhysicalDevice vulkan::context::device::PhysicalDevice::pick_physical_device(
    const vulkan::context::Instance &instance, const Surface &surface) const {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    for (const VkPhysicalDevice &device : devices) {
        if (is_device_suitable(device, surface)) {
            physical_device = device;
            break;
        }
    }

    if (physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    /*print_device_version(physical_device);*/

    return physical_device;
}

bool vulkan::context::device::PhysicalDevice::is_device_suitable(
    const VkPhysicalDevice &physical_device, const Surface &surface) const {
    // NOTE: For mor info check:
    // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
    /*VkPhysicalDeviceProperties deviceProperties;*/
    /*vkGetPhysicalDeviceProperties(device, &deviceProperties);*/

    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);

    QueueFamilyIndices indices =
        device::PhysicalDevice::find_queue_families(physical_device, surface);
    bool extensionsSupported = check_device_extension_support(physical_device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        device::SwapChainSupportDetails swap_chain_support =
            query_swap_chain_support(physical_device, surface);

        swapChainAdequate = !swap_chain_support.formats.empty() &&
                            !swap_chain_support.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
           device_features.samplerAnisotropy;
}

bool vulkan::context::device::PhysicalDevice::check_device_extension_support(
    const VkPhysicalDevice &physical_device) const {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensionCount,
                                         nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensionCount,
                                         availableExtensions.data());

    /*print_supported_extensions(availableExtensions);*/

    std::set<std::string> requiredExtensions(device_extensions.begin(),
                                             device_extensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void vulkan::context::device::PhysicalDevice::print_device_version(
    const VkPhysicalDevice &device) const {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    logger::debug("=== Device ", ": ", device_properties.deviceName, " ===");
    logger::debug("API Version: ", VK_VERSION_MAJOR(device_properties.apiVersion), ".",
                  VK_VERSION_MINOR(device_properties.apiVersion), ".",
                  VK_VERSION_PATCH(device_properties.apiVersion));
    logger::debug("Driver Version: ", device_properties.driverVersion);
}

void vulkan::context::device::PhysicalDevice::print_supported_extensions(
    const std::vector<VkExtensionProperties> &extensions) const {
    // Print all supported extensions
    logger::info("Supported physical device extensions:");
    for (const auto &ext : extensions) {
        logger::info(ext.extensionName);
    }

    /*bool hasDescriptorIndexing = false;*/
    /*for (const auto &ext : extensions) {*/
    /*    if (std::string(ext.extensionName) == VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME)
     * {*/
    /*        hasDescriptorIndexing = true;*/
    /*        break;*/
    /*    }*/
    /*}*/
    /*std::cout << "VK_EXT_descriptor_indexing extension: "*/
    /*          << (hasDescriptorIndexing ? "SUPPORTED" : "NOT SUPPORTED") << "\n\n";*/
}

vulkan::context::device::QueueFamilyIndices
vulkan::context::device::PhysicalDevice::find_queue_families(
    const Surface &surface) const {
    return find_queue_families(m_physical_device, surface);
}

vulkan::context::device::QueueFamilyIndices
vulkan::context::device::PhysicalDevice::find_queue_families(
    const VkPhysicalDevice &physical_device, const Surface &surface) {
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                             queue_families.data());

    int i = 0;
    for (const auto &queue_family : queue_families) {
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface,
                                             &present_support);
        if (present_support) {
            indices.presentFamily = i;
        }

        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
        i++;
    }

    return indices;
}

vulkan::context::device::SwapChainSupportDetails
vulkan::context::device::PhysicalDevice::query_swap_chain_support(
    const Surface &surface) const {
    return query_swap_chain_support(m_physical_device, surface);
}

vulkan::context::device::SwapChainSupportDetails
vulkan::context::device::PhysicalDevice::query_swap_chain_support(
    const VkPhysicalDevice &physical_device, const Surface &surface) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface,
                                              &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count,
                                         nullptr);
    if (format_count != 0) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count,
                                             details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &presentModeCount,
                                              nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

uint32_t vulkan::context::device::PhysicalDevice::find_memory_type(
    const uint32_t type_filter, const VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) &&
            (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

vulkan::context::device::LogicalDevice::LogicalDevice(
    const std::vector<const char *> &validation_layers, const Surface &surface,
    const PhysicalDevice &physical_device)
    : m_enable_validation_layers(validation_layers.size() > 0),
      m_logical_device(create_logical_device(surface, physical_device, device_extensions,
                                             validation_layers)) {}

vulkan::context::device::LogicalDevice::~LogicalDevice() {
    if (m_logical_device == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyDevice(m_logical_device, nullptr);
}

VkDevice vulkan::context::device::LogicalDevice::create_logical_device(
    const Surface &surface, const PhysicalDevice &physical_device,
    const std::vector<const char *> &device_extensions,
    const std::vector<const char *> &validation_layers) {
    QueueFamilyIndices indices = physical_device.find_queue_families(surface);

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

    std::vector<const char *> extendedDeviceExtensions(device_extensions);
    if (m_enable_validation_layers) {
        extendedDeviceExtensions.emplace_back(
            VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME); // needed for macOS
    }

    VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{};
    indexing_features.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexing_features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    indexing_features.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
    indexing_features.shaderUniformBufferArrayNonUniformIndexing = VK_TRUE;
    indexing_features.descriptorBindingVariableDescriptorCount = VK_TRUE;
    indexing_features.runtimeDescriptorArray = VK_TRUE;

    VkPhysicalDeviceFeatures2 devices_features{};
    devices_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    devices_features.pNext = &indexing_features;
    devices_features.features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &devices_features; // new way of enabling features
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = nullptr; // Old way of enabling features

    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(extendedDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = extendedDeviceExtensions.data();

    if (m_enable_validation_layers) {
        // Deprecated on new versions of vulkan, set them here for backwards
        // compatability
        createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        createInfo.ppEnabledLayerNames = validation_layers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkDevice device;
    if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    return device;
}

void vulkan::context::device::LogicalDevice::wait_idle() {
    vkDeviceWaitIdle(m_logical_device);
}

void vulkan::context::device::LogicalDevice::wait_for_fence(const VkFence &fence) const {
    vkWaitForFences(m_logical_device, 1, &fence, VK_TRUE, UINT64_MAX);
}

void vulkan::context::device::LogicalDevice::reset_fence(const VkFence &fence) const {
    vkResetFences(m_logical_device, 1, &fence);
}
