#pragma once

#include "glm/fwd.hpp"
#include <iostream>
#include <sstream>
#include <string>

#ifdef NDEBUG
#define DEBUG_ASSERT(condition, message) ((void)0)
#else
#define DEBUG_ASSERT(condition, message)                                                 \
    do {                                                                                 \
        if (!(condition)) {                                                              \
            std::cerr << "Assertion failed: (" << #condition << "), "                    \
                      << "function " << __FUNCTION__ << ", file " << __FILE__            \
                      << ", line " << __LINE__ << ".\n"                                  \
                      << "Message: " << message << std::endl;                            \
            std::abort();                                                                \
        }                                                                                \
    } while (false)
#endif

std::ostream &operator<<(std::ostream &os, const glm::vec2 &vec);
std::ostream &operator<<(std::ostream &os, const glm::vec3 &vec);
std::ostream &operator<<(std::ostream &os, const glm::vec4 &vec);
std::ostream &operator<<(std::ostream &os, const std::vector<glm::vec3> &vec);
std::ostream &operator<<(std::ostream &os, const std::vector<uint16_t> &us);

std::ostream &operator<<(std::ostream &os, const glm::ivec2 &vec);
std::ostream &operator<<(std::ostream &os, const glm::ivec3 &vec);
std::ostream &operator<<(std::ostream &os, const glm::ivec4 &vec);

std::ostream &operator<<(std::ostream &os, const glm::mat4 &m);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    os << "[ ";
    for (auto v : vec) {
        os << v << ", ";
    }
    os << "]";
    return os;
}

namespace io {
template <typename T> std::string to_string(const T &t) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}
} // namespace io
