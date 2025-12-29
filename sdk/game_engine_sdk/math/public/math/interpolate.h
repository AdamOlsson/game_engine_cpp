#pragma once

#include <utility>
namespace math {

template <typename T>
std::pair<T, T> lerp(const std::pair<T, T> &A, const std::pair<T, T> &B, const float t) {
    return std::make_pair((B.first - A.first) * t, (B.second - A.second) * t);
}
} // namespace math
