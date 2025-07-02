#pragma once
#include "glm/glm.hpp"
#include <string>

namespace ui {
// NOTE: Order need to match shaders definition of this struct
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
        alignas(16) glm::vec3 color;
        alignas(4) glm::float32_t rotation;
        alignas(4) glm::uint32_t font_size;
    } font;
};
} // namespace ui
