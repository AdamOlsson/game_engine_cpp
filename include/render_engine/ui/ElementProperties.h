#pragma once
#include "render_engine/colors.h"

namespace ui {
struct ElementProperties {
    struct ContainerProperties {
        glm::vec2 center;
        glm::vec2 dimension;
        struct {
            glm::vec3 color;
            float thickness;
            float radius;
        } border;
    } container;

    struct FontProperties {
        alignas(16) glm::vec3 color = colors::WHITE; // 16
        alignas(4) glm::uint32_t size = 128;         // 24
        alignas(4) glm::float32_t rotation = 0.0f;   // 20
        alignas(4) glm::float32_t weight = 0.4f;     // 28
        alignas(4) glm::float32_t sharpness = 2.5f;  // 32
    } font;
};
} // namespace ui
