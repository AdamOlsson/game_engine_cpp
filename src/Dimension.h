#pragma once
#include "glm/glm.hpp"
#include "vulkan/vulkan_core.h"
#include <cstdint>

struct Dimension : public glm::vec2 {
    using glm::vec2::vec2;

    Dimension(uint32_t width, uint32_t height)
        : glm::vec2(static_cast<float>(width), static_cast<float>(height)) {}

    Dimension(float width, float height) : glm::vec2(width, height) {}

    Dimension(float diameter) : glm::vec2(diameter, diameter) {}

    static Dimension from_extent2d(VkExtent2D &extent) {
        return Dimension{extent.width, extent.height};
    }

    float width() const { return this->x; }
    float height() const { return this->y; }
};
