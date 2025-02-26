#pragma once
#include "physics_engine/RigidBody.h"
#include <glm/glm.hpp>
#include <optional>

enum class ContactType { NONE, VERTEX_VERTEX, VERTEX_EDGE, EDGE_EDGE };
std::ostream &operator<<(std::ostream &os, const ContactType &c);

struct CollisionInformation {
    float penetration_depth;
    glm::vec3 normal;
    ContactType contact_type;
    std::vector<glm::vec3> contact_patch;
    size_t deepest_contact_idx;
};

std::ostream &operator<<(std::ostream &os, const CollisionInformation &ci);

class SAT {
  public:
    SAT() = default;
    ~SAT() = default;

    static std::optional<CollisionInformation> collision_detection(const RigidBody &,
                                                                   const RigidBody &);
};

std::ostream &operator<<(std::ostream &os, const CollisionInformation &ci);
