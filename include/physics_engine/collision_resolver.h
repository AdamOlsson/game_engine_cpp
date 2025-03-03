#pragma once

#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"

struct Correction {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    float angular_velocity = 0.0f;
};

struct CollisionCorrections {
    Correction body_a = Correction{};
    Correction body_b = Correction{};
};

std::ostream &operator<<(std::ostream &os, const Correction &c);
std::ostream &operator<<(std::ostream &os, const CollisionCorrections &c);

std::optional<CollisionCorrections>
resolve_collision(const CollisionInformation &ci, RigidBody &body_a, RigidBody &body_b);
