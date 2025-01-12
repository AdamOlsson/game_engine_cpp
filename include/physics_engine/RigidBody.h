#pragma once

#include "Coordinates.h"
#include "Shape.h"
#include <glm/glm.hpp>
#include <vector>

struct RigidBody {
    WorldPoint position;
    glm::float32_t rotation;
    Shape shape;

    std::vector<glm::vec3> vertices() const;
    std::vector<glm::vec3> normals() const;
    std::vector<glm::vec3> edges() const;

    bool is_point_inside(WorldPoint &) const;

    /*RigidBody triangle(float side) const;*/
};
