#pragma once
#include "game_engine_sdk/io.h"
#include "glm/glm.hpp"
#include <sstream>

namespace graphics_pipeline {

struct QuadPipelineSBO {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    glm::vec4 uvwt = glm::vec4(-1.0f);
    glm::uint32 texture_id = 0; // Smallest unsigned int (bit-wise) glsl supports

    std::string to_string() const {
        std::ostringstream oss;
        oss << "QuadPipelineUBO {\n"
            << "  model_matrix: " << model_matrix << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const QuadPipelineSBO &obj) {
        return os << obj.to_string();
    }
};
} // namespace graphics_pipeline
