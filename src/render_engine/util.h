#pragma once

#include "render_engine/PhysicalDevice.h"
#include "render_engine/Surface.h"
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

QueueFamilyIndices findQueueFamilies(const PhysicalDevice &device,
                                     const Surface &surface);
QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device,
                                     const VkSurfaceKHR &surface);

SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &physicalDevice,
                                              const VkSurfaceKHR &surface);
