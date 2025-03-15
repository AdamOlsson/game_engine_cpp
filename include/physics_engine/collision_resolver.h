#pragma once

#include "physics_engine/RigidBody.h"
#include "physics_engine/SAT.h"

struct Correction {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    float angular_velocity = 0.0f;

    Correction operator-() const { return {-position, -velocity, -angular_velocity}; }
};

struct CollisionCorrections {
    Correction body_a = Correction{};
    Correction body_b = Correction{};
};

std::ostream &operator<<(std::ostream &os, const Correction &c);
std::ostream &operator<<(std::ostream &os, const CollisionCorrections &c);

class CollisionSolver {
  private:
    CollisionSolver(const CollisionSolver &);
    CollisionSolver &operator=(const CollisionSolver &);

    std::optional<CollisionCorrections>
    resolve_collision_edge_edge(const CollisionInformation &ci, const RigidBody &body_a,
                                const RigidBody &body_b);
    std::optional<CollisionCorrections>
    resolve_collision_vertex_vertex(const CollisionInformation &ci,
                                    const RigidBody &body_a, const RigidBody &body_b);

  public:
    float baumgarte_factor;

    CollisionSolver();
    CollisionSolver(float baumgarte_factor);
    ~CollisionSolver() = default;
    std::optional<CollisionCorrections> resolve_collision(const CollisionInformation &ci,
                                                          const RigidBody &body_a,
                                                          const RigidBody &body_b);
};
