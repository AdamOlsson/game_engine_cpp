#pragma once

#include "glm/fwd.hpp"
#include <concepts>
#include <utility>

namespace math {

template <typename T>
concept StaticCastableToFloat = requires(T t) {
    { static_cast<float>(t) };
};

template <typename T>
concept Vec2Or3 = std::same_as<T, glm::vec2> || std::same_as<T, glm::vec3>;

template <typename T>
concept Point =
    std::same_as<T, glm::vec2> || std::same_as<T, glm::vec3> ||
    std::same_as<T, std::pair<float, float>> ||
    std::same_as<T, std::pair<double, double>> || std::same_as<T, std::pair<int, int>> ||
    std::same_as<T, std::pair<unsigned int, unsigned int>>;

} // namespace math
