#include "io.h"
#include "physics_engine/SAT.h"
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>

std::string vec_to_string(const glm::vec3 &vec) {
    std::ostringstream oss;
    oss << "glm::vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return oss.str();
}

std::ostream &operator<<(std::ostream &os, const glm::vec3 &vec) {
    os << "glm::vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

std::string vec_to_string(const glm::vec2 &vec) {
    std::ostringstream oss;
    oss << "glm::vec2(" << vec.x << ", " << vec.y << ")";
    return oss.str();
}

std::string mat_to_string(const glm::mat3x3 &mat) {
    std::ostringstream oss;
    oss << "glm::mat3x3(" << std::endl
        << mat[0].x << ", " << mat[0].y << ", " << mat[0].z << std::endl
        << mat[1].x << ", " << mat[1].y << ", " << mat[1].z << std::endl
        << mat[2].x << ", " << mat[2].y << ", " << mat[2].z << std::endl
        << ")";
    return oss.str();
}

std::ostream &operator<<(std::ostream &os, const CollisionInformation &ci) {
    os << "CollisionInformation( point: " << ci.collision_point
       << ", normal: " << ci.normal << ", depth: " << ci.penetration_depth << ")";
    return os;
}
