#include "game_engine_sdk/render_engine/vulkan/Instance.h"
#include "game_engine_sdk/render_engine/graphics_context/validation_layers.h"
#include "logger/logger.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

vulkan::Instance::Instance(bool enable_validation_layers)
    : m_enable_validation_layers(enable_validation_layers),
      m_instance(create_instance()) {}

vulkan::Instance::~Instance() { vkDestroyInstance(m_instance, nullptr); }

VkInstance vulkan::Instance::create_instance() {
    print_vulkan_version();
    if (m_enable_validation_layers &&
        !graphics_context::validation_layers::check_validation_layer_support()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.pNext = nullptr;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (m_enable_validation_layers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(
            graphics_context::validation_layers::validation_layers.size());
        createInfo.ppEnabledLayerNames =
            graphics_context::validation_layers::validation_layers.data();
        graphics_context::validation_layers::messenger::DebugMessenger::
            populate_debug_messenger_create_info(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    auto extensions = get_required_extensions();
    /*print_enabled_extensions(extensions);*/
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    /*print_supported_extensions();*/
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

void vulkan::Instance::print_enabled_extensions(
    const std::vector<const char *> &extensions) {
    logger::debug("Enabled extensions are:");
    for (auto e : extensions) {
        logger::debug(e);
    }
}

std::vector<const char *> vulkan::Instance::get_required_extensions() {
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
    /*extensions.emplace_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);*/
    return extensions;
}

void vulkan::Instance::print_vulkan_version() {
    uint32_t instanceVersion;
    VkResult result = vkEnumerateInstanceVersion(&instanceVersion);

    if (result == VK_SUCCESS) {
        uint32_t major = VK_VERSION_MAJOR(instanceVersion);
        uint32_t minor = VK_VERSION_MINOR(instanceVersion);
        uint32_t patch = VK_VERSION_PATCH(instanceVersion);

        logger::debug("Vulkan Instance Version: ", major, ".", minor, ".", patch);
    }
}

void vulkan::Instance::print_supported_extensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    // Print all supported extensions
    logger::info("Suppoerted instance extensions:");
    for (const auto &ext : extensions) {
        logger::info(ext.extensionName);
    }
}
