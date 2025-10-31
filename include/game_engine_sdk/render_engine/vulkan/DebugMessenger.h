#pragma once

#include "Instance.h"
#include "vulkan/vulkan_core.h"
namespace vulkan {

class DebugMessenger {
  private:
    Instance *m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;

    VkDebugUtilsMessengerEXT setup_debug_messenger();

    VkResult create_debug_utils_messenger_ext(
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        VkDebugUtilsMessengerEXT *pDebugMessenger);

    void destroy_debug_messenger_ext();

  public:
    DebugMessenger(vulkan::Instance *instance);
    ~DebugMessenger();

    DebugMessenger(DebugMessenger &&other) noexcept
        : m_instance(other.m_instance), m_debug_messenger(other.m_debug_messenger) {
        other.m_debug_messenger = VK_NULL_HANDLE; // Prevent double destruction
        other.m_instance = nullptr;
    }

    DebugMessenger &operator=(DebugMessenger &&other) noexcept {
        if (this != &other) {
            destroy_debug_messenger_ext(); // Clean up current resources
            m_instance = other.m_instance;
            m_debug_messenger = other.m_debug_messenger;
            other.m_debug_messenger = VK_NULL_HANDLE;
            other.m_instance = nullptr;
        }
        return *this;
    }
    DebugMessenger(const DebugMessenger &) = delete;
    DebugMessenger &operator=(const DebugMessenger &) = delete;

    static void
    populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info);
};

}; // namespace vulkan
