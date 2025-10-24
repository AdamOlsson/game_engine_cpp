#pragma once

#include "glm/glm.hpp"
#include <ostream>

struct UVWT : public glm::vec4 {
    using glm::vec4::vec4;

    UVWT() = default;
    ~UVWT() = default;
    UVWT(float u, float v, float w, float t) : glm::vec4(u, v, w, t) {}
    UVWT(const glm::vec4 &vec) : glm::vec4(vec) {}

    friend std::ostream &operator<<(std::ostream &os, const UVWT &uvwt) {
        os << "UVWT(" << uvwt.x << ", " << uvwt.y << ", " << uvwt.z << ", " << uvwt.w
           << ")";
        return os;
    }
};
