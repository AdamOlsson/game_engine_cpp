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
  private:
    static std::optional<CollisionInformation>
    collision_detection_polygon(const RigidBody &body_a, const RigidBody &body_b);

    static std::optional<CollisionInformation>
    collision_detection_circle(const RigidBody &body_a, const RigidBody &body_b);

    static std::optional<CollisionInformation>
    collision_detection_circle_polygon(const RigidBody &body_a, const RigidBody &body_b);

  public:
    SAT() = default;
    ~SAT() = default;

    static std::optional<CollisionInformation> collision_detection(const RigidBody &,
                                                                   const RigidBody &);
};

std::ostream &operator<<(std::ostream &os, const CollisionInformation &ci);
