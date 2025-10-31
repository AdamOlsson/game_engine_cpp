#include "vulkan/context/DebugMessenger.h"
#include <iostream>

VKAPI_ATTR inline VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

vulkan::context::DebugMessenger::DebugMessenger(Instance *instance)
    : m_instance(instance), m_debug_messenger(setup_debug_messenger()) {}

vulkan::context::DebugMessenger::~DebugMessenger() {
    if (m_debug_messenger == VK_NULL_HANDLE) {
        return;
    }

    destroy_debug_messenger_ext();

    m_debug_messenger = VK_NULL_HANDLE;
}

VkDebugUtilsMessengerEXT vulkan::context::DebugMessenger::setup_debug_messenger() {

    VkDebugUtilsMessengerCreateInfoEXT create_info;
    vulkan::context::DebugMessenger::populate_debug_messenger_create_info(create_info);

    VkDebugUtilsMessengerEXT debug_messenger;
    if (create_debug_utils_messenger_ext(&create_info, &debug_messenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
    return debug_messenger;
}

VkResult vulkan::context::DebugMessenger::create_debug_utils_messenger_ext(
    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        *m_instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(*m_instance, pCreateInfo, nullptr, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void vulkan::context::DebugMessenger::destroy_debug_messenger_ext() {
    if (m_debug_messenger == VK_NULL_HANDLE) {
        return;
    }

    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        *m_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(*m_instance, m_debug_messenger, nullptr);
    }
}

void vulkan::context::DebugMessenger::populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &create_info) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debug_callback;
    create_info.pUserData = nullptr; // Optional
}
