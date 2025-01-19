#include "vulkan/vulkan_core.h"
#include <optional>
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device,
                                     VkSurfaceKHR &surface);

SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &physicalDevice,
                                              VkSurfaceKHR &surface);
