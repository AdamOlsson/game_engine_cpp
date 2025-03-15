#pragma once
#include "physics_engine/RigidBody.h"

std::vector<glm::vec3>
get_triangle_vertices(const RigidBody &body,
                      const glm::vec3 &translate = glm::vec3(0.0, 0.0, 0.0),
                      const float rotate = 0.0);
std::vector<glm::vec3> get_triangle_edges(const RigidBody &body);
std::vector<glm::vec3> get_triangle_normals(const RigidBody &body);
float get_rectangle_bounding_volume_radius(const RigidBody &body);
bool is_point_inside_triangle(const RigidBody &body, const WorldPoint &point);
WorldPoint closest_point_on_triangle(const RigidBody &body, const WorldPoint &point);
float triangle_inertia(const RigidBody &body);
