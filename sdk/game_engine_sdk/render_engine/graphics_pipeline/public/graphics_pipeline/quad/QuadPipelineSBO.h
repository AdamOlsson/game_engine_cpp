#pragma once
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "logger/io.h"
#include "util/colors.h"
#include <glm/glm.hpp>
#include <sstream>

namespace graphics_pipeline::quad {

class QuadRenderer;

struct QuadPipelineSBO {
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
    alignas(16) glm::vec4 uvwt = glm::vec4(-1.0f);
    alignas(4) glm::uint32 texture_id = 0;
    alignas(16) glm::vec4 color = util::colors::TRANSPARENT;

    std::string to_string() const {
        std::ostringstream oss;
        oss << "QuadPipelineUBO {\n"
            << "  model_matrix: " << model_matrix << "\n"
            << "  uvxt: " << uvwt << "\n"
            << "  texture_id: " << texture_id << "\n"
            << "  color: " << color << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const QuadPipelineSBO &obj) {
        return os << obj.to_string();
    }
};

class QuadSBOHandle {
  private:
    friend class QuadRenderer;

    size_t id = std::numeric_limits<size_t>::max();
    std::optional<std::function<void(size_t)>> return_fn = std::nullopt;

  public:
    QuadPipelineSBO *data = nullptr;

    QuadSBOHandle() = default;
    QuadSBOHandle(size_t id, QuadPipelineSBO *data, std::function<void(size_t)> return_fn)
        : id(id), return_fn(return_fn), data(data) {
        DEBUG_ASSERT(data != nullptr,
                     "Error: Created QuadSBOHandle with a nullptr to data.");
    }

    QuadSBOHandle(const QuadSBOHandle &) = delete;
    QuadSBOHandle(QuadSBOHandle &&other) noexcept
        : id(other.id), return_fn(std::move(other.return_fn)), data(other.data) {
        other.data = nullptr;
        other.return_fn = std::nullopt;
    }

    QuadSBOHandle &operator=(const QuadSBOHandle &&) = delete;
    QuadSBOHandle &operator=(QuadSBOHandle &&other) noexcept {
        if (this != &other) {
            // Return our current slot if we have one
            return_to_source();

            // Take ownership of the other's slot
            id = other.id;
            return_fn = std::move(other.return_fn);
            data = other.data;

            other.data = nullptr; // Prevent the moved-from object from returning the slot
            other.return_fn = std::nullopt;
        }
        return *this;
    }

    ~QuadSBOHandle() { return_to_source(); }

    void return_to_source() {
        if (!return_fn.has_value()) {
            return;
        }
        return_fn.value()(id);
        data = nullptr;
    }
};

} // namespace graphics_pipeline::quad
