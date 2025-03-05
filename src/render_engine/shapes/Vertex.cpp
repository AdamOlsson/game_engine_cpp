#include "Vertex.h"

std::ostream &operator<<(std::ostream &os, const Vertex &v) {
    return os << "Vertex(" << v.position.x << ", " << v.position.y << ", " << v.position.z
              << ")";
}

std::ostream &operator<<(std::ostream &os, const std::vector<Vertex> &vs) {
    os << "[";
    for (auto v : vs) {
        os << v << ", ";
    }
    return os << "]";
}
