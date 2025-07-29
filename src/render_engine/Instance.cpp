#include "Instance.h"
#include "render_engine/validation_layers.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

Instance::Instance(bool enable_validation_layers)
    : m_enable_validation_layers(enable_validation_layers),
      m_instance(create_instance()) {}

Instance::~Instance() { vkDestroyInstance(m_instance, nullptr); }

VkInstance Instance::create_instance() {
    if (m_enable_validation_layers &&
        !validation_layers::check_validation_layer_support()) {
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
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validation_layers::validation_layers.size());
        createInfo.ppEnabledLayerNames = validation_layers::validation_layers.data();
        validation_layers::messenger::DebugMessenger::
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

std::vector<const char *> Instance::get_required_extensions() {
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

void Instance::print_enabled_extensions() {
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
