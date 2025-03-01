#pragma once

#include "glm/fwd.hpp"
#include <iostream>
#include <string>

std::string vec_to_string(const glm::vec3 &vec);
std::string vec_to_string(const glm::vec2 &vec);
std::string mat_to_string(const glm::mat3 &mat);

std::ostream &operator<<(std::ostream &os, const glm::vec3 &vec);
std::ostream &operator<<(std::ostream &os, const std::vector<glm::vec3> &vec);
