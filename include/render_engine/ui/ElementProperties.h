#pragma once
#include "render_engine/colors.h"

namespace ui {
struct ElementProperties {
    struct ContainerProperties {
        alignas(8) glm::vec2 center = glm::vec2(0.0f, 0.0f);
        alignas(8) glm::vec2 dimension = glm::vec2(0.0f, 0.0f);
        alignas(16) glm::vec4 background_color = colors::TRANSPARENT;
        struct {
            alignas(16) glm::vec4 color = colors::WHITE;
            alignas(4) float thickness = 0.0f;
            alignas(4) float radius = 0.0f;
        } border;
    } container;

    struct FontProperties {
        alignas(16) glm::vec4 color = colors::WHITE; // 16
        alignas(4) glm::uint32_t size = 128;         // 24
        alignas(4) glm::float32_t rotation = 0.0f;   // 20
        alignas(4) glm::float32_t weight = 0.4f;     // 28
        alignas(4) glm::float32_t sharpness = 2.3f;  // 32
    } font;
};
} // namespace ui
