#pragma once

#include <array>
#include <iostream>
#include <sstream>
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace vertex {
constexpr uint32_t MAX_VERTICES = 64;
enum class VertexShape : uint32_t { Circle = 0, Rectangle = 1, Polygon = 2 };
} // namespace vertex

struct alignas(16) Vertex : public glm::vec3 {
    using glm::vec3::vec3;

    constexpr Vertex() : glm::vec3(0.0f) {}
    constexpr Vertex(const float x, const float y, const float z) : glm::vec3(x, y, z) {}
    constexpr Vertex(const glm::vec3 &v) : glm::vec3(v) {}

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = 0; // offsetof(Vertex, position);

        return attributeDescriptions;
    }

    friend std::ostream &operator<<(std::ostream &os, const Vertex &v) {
        return os << "Vertex(" << v.x << ", " << v.y << ", " << v.z << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const std::vector<Vertex> &vs) {
        os << "[";
        for (auto v : vs) {
            os << v << ", ";
        }
        return os << "]";
    }
};

struct alignas(16) VertexUBO {
    Vertex vertices[vertex::MAX_VERTICES]{};
    uint32_t num_vertices{};
    uint32_t max_vertices = vertex::MAX_VERTICES;
    uint32_t shape = static_cast<uint32_t>(vertex::VertexShape::Polygon);
    uint32_t _pad = 0;

    std::string to_string() const {
        std::ostringstream oss;
        oss << "VertexUBO{\n";
        oss << "  num_vertices: " << num_vertices << "\n";
        oss << "  max_vertices: " << max_vertices << "\n";
        oss << "  vertices: [\n";

        for (uint32_t i = 0; i < num_vertices; ++i) {
            oss << "    [" << i << "]: (" << vertices[i] << ")";
            if (i < num_vertices - 1) {
                oss << ",";
            }
            oss << "\n";
        }

        oss << "  ]\n}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const VertexUBO &ubo) {
        return os << ubo.to_string();
    }
};
