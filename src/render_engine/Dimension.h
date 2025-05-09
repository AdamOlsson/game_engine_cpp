#pragma once
#include "vulkan/vulkan_core.h"
#include <cstdint>

struct Dimension {
    uint32_t width;
    uint32_t height;
    Dimension(uint32_t width, uint32_t height) : width(width), height(height) {}
    static Dimension from_extent2d(VkExtent2D &extent) {
        return Dimension{extent.width, extent.height};
    }
};
