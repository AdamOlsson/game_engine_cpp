#include "logger/io.h"
#include <glm/glm.hpp>
#include <iostream>

std::ostream &operator<<(std::ostream &os, const glm::vec2 &vec) {
    os << "glm::vec2(" << vec.x << ", " << vec.y << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const glm::vec3 &vec) {
    os << "glm::vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const glm::vec4 &vec) {
    os << "glm::vec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<glm::vec3> &vec) {
    os << "[ ";
    for (auto v : vec) {
        os << v << ", ";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<uint16_t> &us) {
    os << "[";
    for (auto u : us) {
        os << u << ", ";
    }
    return os << "]";
}

std::ostream &operator<<(std::ostream &os, const glm::ivec2 &vec) {
    os << "glm::ivec2(" << vec.x << ", " << vec.y << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const glm::ivec3 &vec) {
    os << "glm::ivec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const glm::ivec4 &vec) {
    os << "glm::ivec4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w
       << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const glm::mat4 &m) {
    return os << std::format("glm::mat4 {{\n"
                             "  {:.3f} {:.3f} {:.3f} {:.3f}\n"
                             "  {:.3f} {:.3f} {:.3f} {:.3f}\n"
                             "  {:.3f} {:.3f} {:.3f} {:.3f}\n"
                             "  {:.3f} {:.3f} {:.3f} {:.3f}\n"
                             "}}",
                             m[0].x, m[0].y, m[0].z, m[0].w, m[1].x, m[1].y, m[1].z,
                             m[1].w, m[2].x, m[2].y, m[2].z, m[2].w, m[3].x, m[3].y,
                             m[3].z, m[3].w);
}
