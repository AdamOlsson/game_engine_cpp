#pragma once

#include "glm/glm.hpp"

#pragma once
#include <glm/glm.hpp>

namespace colors {
// Primary Colors
constexpr glm::vec3 RED = glm::vec3(0.8f, 0.0f, 0.0f);
constexpr glm::vec3 GREEN = glm::vec3(0.0f, 0.8f, 0.0f);
constexpr glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 0.8f);

// Basic Colors
constexpr glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 BLACK = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 GRAY = glm::vec3(0.5f, 0.5f, 0.5f);
constexpr glm::vec3 LIGHT_GRAY = glm::vec3(0.75f, 0.75f, 0.75f);
constexpr glm::vec3 DARK_GRAY = glm::vec3(0.25f, 0.25f, 0.25f);

// Secondary Colors
constexpr glm::vec3 YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 CYAN = glm::vec3(0.0f, 1.0f, 1.0f);
constexpr glm::vec3 MAGENTA = glm::vec3(1.0f, 0.0f, 1.0f);

// Extended Colors
constexpr glm::vec3 ORANGE = glm::vec3(1.0f, 0.65f, 0.0f);
constexpr glm::vec3 PURPLE = glm::vec3(0.5f, 0.0f, 0.5f);
constexpr glm::vec3 PINK = glm::vec3(1.0f, 0.75f, 0.8f);
constexpr glm::vec3 BROWN = glm::vec3(0.65f, 0.16f, 0.16f);
constexpr glm::vec3 LIME = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 NAVY = glm::vec3(0.0f, 0.0f, 0.5f);
constexpr glm::vec3 TEAL = glm::vec3(0.0f, 0.5f, 0.5f);
constexpr glm::vec3 OLIVE = glm::vec3(0.5f, 0.5f, 0.0f);
constexpr glm::vec3 MAROON = glm::vec3(0.5f, 0.0f, 0.0f);

// Pastel Colors
constexpr glm::vec3 LIGHT_BLUE = glm::vec3(0.68f, 0.85f, 0.9f);
constexpr glm::vec3 LIGHT_GREEN = glm::vec3(0.56f, 0.93f, 0.56f);
constexpr glm::vec3 LIGHT_RED = glm::vec3(1.0f, 0.71f, 0.76f);
constexpr glm::vec3 LIGHT_YELLOW = glm::vec3(1.0f, 1.0f, 0.88f);
constexpr glm::vec3 LIGHT_PURPLE = glm::vec3(0.87f, 0.63f, 0.87f);
constexpr glm::vec3 LIGHT_ORANGE = glm::vec3(1.0f, 0.84f, 0.7f);

// Dark Colors
constexpr glm::vec3 DARK_RED = glm::vec3(0.55f, 0.0f, 0.0f);
constexpr glm::vec3 DARK_GREEN = glm::vec3(0.0f, 0.39f, 0.0f);
constexpr glm::vec3 DARK_BLUE = glm::vec3(0.0f, 0.0f, 0.55f);
constexpr glm::vec3 DARK_ORANGE = glm::vec3(1.0f, 0.55f, 0.0f);
constexpr glm::vec3 DARK_PURPLE = glm::vec3(0.3f, 0.0f, 0.3f);

// Named Web Colors
constexpr glm::vec3 GOLD = glm::vec3(1.0f, 0.84f, 0.0f);
constexpr glm::vec3 SILVER = glm::vec3(0.75f, 0.75f, 0.75f);
constexpr glm::vec3 CRIMSON = glm::vec3(0.86f, 0.08f, 0.24f);
constexpr glm::vec3 FOREST_GREEN = glm::vec3(0.13f, 0.55f, 0.13f);
constexpr glm::vec3 ROYAL_BLUE = glm::vec3(0.25f, 0.41f, 0.88f);
constexpr glm::vec3 CORAL = glm::vec3(1.0f, 0.5f, 0.31f);
constexpr glm::vec3 TURQUOISE = glm::vec3(0.25f, 0.88f, 0.82f);
constexpr glm::vec3 VIOLET = glm::vec3(0.93f, 0.51f, 0.93f);
constexpr glm::vec3 INDIGO = glm::vec3(0.29f, 0.0f, 0.51f);
constexpr glm::vec3 KHAKI = glm::vec3(0.94f, 0.9f, 0.55f);

consteval glm::vec3 rgb(float r, float g, float b) noexcept { return glm::vec3(r, g, b); }

consteval glm::vec3 rgb255(int r, int g, int b) noexcept {
    return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

consteval glm::vec3 hex(uint32_t color) noexcept {
    return glm::vec3(((color >> 16) & 0xFF) / 255.0f, ((color >> 8) & 0xFF) / 255.0f,
                     (color & 0xFF) / 255.0f);
}
} // namespace colors
