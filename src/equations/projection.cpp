#include "equations/projection.h"
#include <iostream>

bool operator==(const Projection &lhs, const Projection &rhs) {
    constexpr float epsilon = 1e-6f; // For floating-point comparison
    return std::fabs(lhs.min - rhs.min) < epsilon &&
           std::fabs(lhs.max - rhs.max) < epsilon;
}

std::ostream &operator<<(std::ostream &os, const Projection &projection) {
    os << "Projection(min: " << projection.min << ", max: " << projection.max << ")";
    return os;
}

Projection Projection::project_body_on_axis(const RigidBody &body,
                                            const glm::vec3 &axis) {
    std::vector<glm::vec3> corners = body.vertices();

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();

    for (const auto &corner : corners) {
        float projection = glm::dot(axis, corner);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }

    return Projection{min, max};
}

Overlap Projection::overlap(const Projection &other) const {
    float max_val = std::min(this->max, other.max);
    float min_val = std::max(this->min, other.min);
    return Overlap{max_val - min_val};
}
