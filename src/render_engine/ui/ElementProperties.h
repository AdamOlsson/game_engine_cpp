#pragma once
#include "glm/glm.hpp"

namespace ui {

struct ElementProperties {
    glm::vec2 center;
    glm::vec2 dimension;
    struct {
        float radius;
        float thickness;
    } border;
};
} // namespace ui
