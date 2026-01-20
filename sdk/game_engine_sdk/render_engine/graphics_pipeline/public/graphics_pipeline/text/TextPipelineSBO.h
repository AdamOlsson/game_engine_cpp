#pragma once
#include "logger/io.h"
#include <glm/glm.hpp>
#include <sstream>

namespace graphics_pipeline::text {

struct TextPipelineSBO {
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);

    std::string to_string() const {
        std::ostringstream oss;
        oss << "TextPipelineUBO {\n"
            << "  model_matrix: " << model_matrix << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const TextPipelineSBO &obj) {
        return os << obj.to_string();
    }
};
} // namespace graphics_pipeline::text
