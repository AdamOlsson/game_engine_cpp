#include "CoreGraphicsContext.h"
#include "render_engine/window/Window.h"
#include "util.h"
#include "vulkan/vulkan_beta.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <set>
#include <stdexcept>

namespace {
const std::vector<const char *> validation_layers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

} // namespace

CoreGraphicsContext::CoreGraphicsContext(window::Window *window)
    : m_enable_validation_layers(true), window(window), instance(create_instance()),
      m_debug_messenger(setup_debug_messenger()),
      surface(create_surface(*this->window->window)),
      physical_device(pick_physical_device(instance, surface)),
      device(create_logical_device(device_extensions)) {}

CoreGraphicsContext::~CoreGraphicsContext() {
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);

    if (m_enable_validation_layers) {
        destroy_debug_messenger_ext();
    }

    vkDestroyInstance(instance, nullptr);
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

VKAPI_ATTR inline VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

bool CoreGraphicsContext::check_validation_layer_support() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validation_layers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> CoreGraphicsContext::get_required_extensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    if (m_enable_validation_layers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        // required for VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }

    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    return extensions;
}

void CoreGraphicsContext::print_enabled_extensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    // Print all supported extensions
    std::cout << "Available m_instance extensions:" << std::endl;
    for (const auto &ext : extensions) {
        std::cout << ext.extensionName << std::endl;
    }
}

VkInstance CoreGraphicsContext::create_instance() {
    if (m_enable_validation_layers && !check_validation_layer_support()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.pNext = nullptr;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (m_enable_validation_layers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        createInfo.ppEnabledLayerNames = validation_layers.data();
        populate_debug_messenger_create_info(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    auto extensions = get_required_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    /*print_enabled_extensions(m_instance);*/
    switch (result) {
    case VK_SUCCESS:
        break;
    case VK_ERROR_LAYER_NOT_PRESENT:
        throw std::runtime_error("layer not present!");
        break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        throw std::runtime_error("extension not present!");
    default:
        throw std::runtime_error(
            "failed to create m_instance! (code: " + std::to_string(result) + ")");
    }
    return instance;
}

void CoreGraphicsContext::destroy_debug_messenger_ext() {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, m_debug_messenger.value(), nullptr);
    }
}

void CoreGraphicsContext::populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}

VkResult CoreGraphicsContext::create_debug_utils_messenger_ext(
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, nullptr, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

std::optional<VkDebugUtilsMessengerEXT> CoreGraphicsContext::setup_debug_messenger() {
    if (!m_enable_validation_layers) {
        return std::nullopt;
    }

    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messenger_create_info(create_info);

    VkDebugUtilsMessengerEXT debug_messenger;
    if (create_debug_utils_messenger_ext(&create_info, &debug_messenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
    return debug_messenger;
}

VkSurfaceKHR CoreGraphicsContext::create_surface(GLFWwindow &window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, &window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
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

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    bool extensionsSupported = check_device_extension_support(physicalDevice);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport =
            querySwapChainSupport(physicalDevice, surface);
        swapChainAdequate =
            !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
           deviceFeatures.samplerAnisotropy;
}

VkPhysicalDevice CoreGraphicsContext::pick_physical_device(VkInstance &m_instance,
                                                           VkSurfaceKHR &surface) {
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
