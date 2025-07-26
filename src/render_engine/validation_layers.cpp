#include "validation_layers.h"

bool validation_layers::check_validation_layer_support() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for (const char *layer_name : validation_layers::validation_layers) {
        bool layer_found = false;

        for (const auto &layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }

    return true;
}

validation_layers::messenger::DebugMessenger::DebugMessenger(Instance &instance)
    : m_instance(&instance), m_debug_messenger(setup_debug_messenger()) {}

validation_layers::messenger::DebugMessenger::~DebugMessenger() {
    destroy_debug_messenger_ext();
}

VkDebugUtilsMessengerEXT
validation_layers::messenger::DebugMessenger::setup_debug_messenger() {

    VkDebugUtilsMessengerCreateInfoEXT create_info;
    validation_layers::messenger::DebugMessenger::populate_debug_messenger_create_info(
        create_info);

    VkDebugUtilsMessengerEXT debug_messenger;
    if (create_debug_utils_messenger_ext(&create_info, &debug_messenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
    return debug_messenger;
}

VkResult validation_layers::messenger::DebugMessenger::create_debug_utils_messenger_ext(
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        m_instance->instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(m_instance->instance, pCreateInfo, nullptr, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void validation_layers::messenger::DebugMessenger::destroy_debug_messenger_ext() {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        m_instance->instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(m_instance->instance, m_debug_messenger.value(), nullptr);
    }
}

void validation_layers::messenger::DebugMessenger::populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &create_info) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = validation_layers::debugCallback;
    create_info.pUserData = nullptr; // Optional
}
