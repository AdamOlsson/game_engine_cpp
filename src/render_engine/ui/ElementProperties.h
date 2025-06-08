#pragma once
#include "glm/glm.hpp"

/*enum class AnimateProperty : uint32_t {*/
/*    NONE = 1 << 0,*/
/*    POSITION = 1 << 1,*/
/*    ROTATION = 1 << 2,*/
/*    COLOR = 1 << 3,*/
/*    DIMENSION = 1 << 4,*/
/*};*/

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

    /*ElementProperties() = default;*/
    /*ElementProperties(const ElementProperties &other) = default;*/
};
} // namespace ui
