#pragma once
#include "glm/glm.hpp"
#include <string>

namespace ui {
// NOTE: Order need to match shaders definition of this struct
struct ElementProperties {

    glm::vec2 center;
    glm::vec2 dimension;
    struct {
        glm::vec3 color;
        float thickness;
        float radius;
    } border;
};
} // namespace ui
