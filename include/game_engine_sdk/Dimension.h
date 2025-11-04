#pragma once
#include "vulkan/vulkan_core.h"
#include <glm/glm.hpp>

struct Dimension : public glm::vec2 {
    using glm::vec2::vec2;

    constexpr Dimension(float width, float height) : glm::vec2(width, height) {}
    constexpr Dimension(float diameter) : glm::vec2(diameter, diameter) {}

    template <typename T>
    constexpr Dimension(T width, T height)
        : glm::vec2(static_cast<float>(width), static_cast<float>(height)) {}

    template <typename T>
    constexpr Dimension(T diameter)
        : glm::vec2(static_cast<float>(diameter), static_cast<float>(diameter)) {}

    static Dimension from_extent2d(VkExtent2D &extent) {
        return Dimension{extent.width, extent.height};
    }

    constexpr float width() const { return this->x; }
    constexpr float height() const { return this->y; }
};
