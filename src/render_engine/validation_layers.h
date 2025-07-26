#pragma once

#include "vulkan/vulkan_core.h"
#include <iostream>
#include <vector>
namespace validation_layers {
const std::vector<const char *> validation_layers = {"VK_LAYER_KHRONOS_validation"};

VKAPI_ATTR inline VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

bool check_validation_layer_support();

namespace messenger {
void populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &create_info);

/*VkResult*/
/*create_debug_utils_messenger_ext(const VkDebugUtilsMessengerCreateInfoEXT
 * *p_create_info,*/
/*                                 VkDebugUtilsMessengerEXT *p_debug_messenger);*/

}; // namespace messenger

}; // namespace validation_layers
