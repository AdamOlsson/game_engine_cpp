#pragma once
/*#include "RigidBodyShape.h"*/
#include "physics_engine/RigidBody.h"

/*namespace PhysicsEngine {*/
/*class Triangle : public RigidBodyShape {};*/
/*} // namespace PhysicsEngine*/
std::vector<glm::vec3>
get_rectangle_vertices(const RigidBody &body,
                       const glm::vec3 &translate = glm::vec3(0.0, 0.0, 0.0),
                       const float rotate = 0.0);
std::vector<glm::vec3> get_rectangle_edges(const RigidBody &body);
std::vector<glm::vec3> get_rectangle_normals(const RigidBody &body);
bool is_point_inside_rectangle(const RigidBody &body, const WorldPoint &point);
WorldPoint closest_point_on_rectangle(const RigidBody &body, const WorldPoint &point);
