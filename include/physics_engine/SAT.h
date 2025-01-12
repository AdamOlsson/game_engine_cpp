#pragma once
#include "physics_engine/RigidBody.h"
#include <glm/glm.hpp>
#include <optional>

struct CollisionInformation {
    float penetration_depth;
    glm::vec3 normal;
    glm::vec3 collision_point;
};

class SAT {
  public:
    SAT() = default;
    ~SAT() = default;

    static std::optional<CollisionInformation> collision_detection(const RigidBody &,
                                                                   const RigidBody &);
};
