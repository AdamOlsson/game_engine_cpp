#pragma once

#include "glm/fwd.hpp"
#include "physics_engine/RigidBody.h"
#include <ostream>

struct Overlap {
    float distance;
};

struct Projection {
    float min;
    float max;

    /*static Projection no_axis(float min, float max) { return Projection{min, max}; }*/

    Overlap overlap(const Projection &) const;
    static Projection project_body_on_axis(const RigidBody &body, const glm::vec3 &axis);
};

std::ostream &operator<<(std::ostream &, const Projection &);
bool operator==(const Projection &, const Projection &);
