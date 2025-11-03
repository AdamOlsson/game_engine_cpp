#pragma once
#include <glm/glm.hpp>
namespace util::colors {
// Primary Colors
constexpr glm::vec4 RED = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
constexpr glm::vec4 GREEN = glm::vec4(0.0f, 0.8f, 0.0f, 1.0f);
constexpr glm::vec4 BLUE = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f);
// Basic Colors
constexpr glm::vec4 WHITE = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr glm::vec4 BLACK = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
constexpr glm::vec4 GRAY = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
constexpr glm::vec4 LIGHT_GRAY = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
constexpr glm::vec4 DARK_GRAY = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
// Transparent Color
constexpr glm::vec4 TRANSPARENT = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
// Secondary Colors
constexpr glm::vec4 YELLOW = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
constexpr glm::vec4 CYAN = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
constexpr glm::vec4 MAGENTA = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
// Extended Colors
constexpr glm::vec4 ORANGE = glm::vec4(1.0f, 0.65f, 0.0f, 1.0f);
constexpr glm::vec4 PURPLE = glm::vec4(0.5f, 0.0f, 0.5f, 1.0f);
constexpr glm::vec4 PINK = glm::vec4(1.0f, 0.75f, 0.8f, 1.0f);
constexpr glm::vec4 BROWN = glm::vec4(0.65f, 0.16f, 0.16f, 1.0f);
constexpr glm::vec4 LIME = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
constexpr glm::vec4 NAVY = glm::vec4(0.0f, 0.0f, 0.5f, 1.0f);
constexpr glm::vec4 TEAL = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
constexpr glm::vec4 OLIVE = glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
constexpr glm::vec4 MAROON = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
// Pastel Colors
constexpr glm::vec4 LIGHT_BLUE = glm::vec4(0.68f, 0.85f, 0.9f, 1.0f);
constexpr glm::vec4 LIGHT_GREEN = glm::vec4(0.56f, 0.93f, 0.56f, 1.0f);
constexpr glm::vec4 LIGHT_RED = glm::vec4(1.0f, 0.71f, 0.76f, 1.0f);
constexpr glm::vec4 LIGHT_YELLOW = glm::vec4(1.0f, 1.0f, 0.88f, 1.0f);
constexpr glm::vec4 LIGHT_PURPLE = glm::vec4(0.87f, 0.63f, 0.87f, 1.0f);
constexpr glm::vec4 LIGHT_ORANGE = glm::vec4(1.0f, 0.84f, 0.7f, 1.0f);
// Dark Colors
constexpr glm::vec4 DARK_RED = glm::vec4(0.55f, 0.0f, 0.0f, 1.0f);
constexpr glm::vec4 DARK_GREEN = glm::vec4(0.0f, 0.39f, 0.0f, 1.0f);
constexpr glm::vec4 DARK_BLUE = glm::vec4(0.0f, 0.0f, 0.55f, 1.0f);
constexpr glm::vec4 DARK_ORANGE = glm::vec4(1.0f, 0.55f, 0.0f, 1.0f);
constexpr glm::vec4 DARK_PURPLE = glm::vec4(0.3f, 0.0f, 0.3f, 1.0f);
// Named Web Colors
constexpr glm::vec4 GOLD = glm::vec4(1.0f, 0.84f, 0.0f, 1.0f);
constexpr glm::vec4 SILVER = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
constexpr glm::vec4 CRIMSON = glm::vec4(0.86f, 0.08f, 0.24f, 1.0f);
constexpr glm::vec4 FOREST_GREEN = glm::vec4(0.13f, 0.55f, 0.13f, 1.0f);
constexpr glm::vec4 ROYAL_BLUE = glm::vec4(0.25f, 0.41f, 0.88f, 1.0f);
constexpr glm::vec4 CORAL = glm::vec4(1.0f, 0.5f, 0.31f, 1.0f);
constexpr glm::vec4 TURQUOISE = glm::vec4(0.25f, 0.88f, 0.82f, 1.0f);
constexpr glm::vec4 VIOLET = glm::vec4(0.93f, 0.51f, 0.93f, 1.0f);
constexpr glm::vec4 INDIGO = glm::vec4(0.29f, 0.0f, 0.51f, 1.0f);
constexpr glm::vec4 KHAKI = glm::vec4(0.94f, 0.9f, 0.55f, 1.0f);

consteval glm::vec4 rgba(float r, float g, float b, float a = 1.0f) noexcept {
    return glm::vec4(r, g, b, a);
}
consteval glm::vec4 rgba255(int r, int g, int b, int a = 255) noexcept {
    return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}
consteval glm::vec4 hex(uint32_t color, float alpha = 1.0f) noexcept {
    return glm::vec4(((color >> 16) & 0xFF) / 255.0f, ((color >> 8) & 0xFF) / 255.0f,
                     (color & 0xFF) / 255.0f, alpha);
}
consteval glm::vec4 hexa(uint32_t color) noexcept {
    return glm::vec4(((color >> 24) & 0xFF) / 255.0f, ((color >> 16) & 0xFF) / 255.0f,
                     ((color >> 8) & 0xFF) / 255.0f, (color & 0xFF) / 255.0f);
}
} // namespace util::colors
