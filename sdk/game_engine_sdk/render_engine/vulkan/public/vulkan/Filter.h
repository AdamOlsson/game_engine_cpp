#pragma once

#include "vulkan/vulkan_core.h"

namespace vulkan {
enum Filter {
    NEAREST = VK_FILTER_NEAREST,
    LINEAR = VK_FILTER_LINEAR,
    CUBIC_EXT = VK_FILTER_CUBIC_EXT,
    CUBIC_IMG = VK_FILTER_CUBIC_IMG,
};
}
