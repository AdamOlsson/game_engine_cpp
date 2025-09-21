#pragma once

#include "game_engine_sdk/physics_engine/RigidBody.h"
#include "glm/fwd.hpp"
#include <ostream>

struct Overlap {
    float distance;
};

struct Projection {
    float min;
    float max;

    Overlap overlap(const Projection &) const;

    /// Projects a polygon onto a given axis.
    static Projection project_polygon_on_axis(const RigidBody &polygon,
                                              const glm::vec3 &axis);

    /// Projects a circle onto a given axis
    static Projection project_circle_on_axis(const RigidBody &circle,
                                             const glm::vec3 &axis);
};

std::ostream &operator<<(std::ostream &, const Projection &);
bool operator==(const Projection &, const Projection &);
