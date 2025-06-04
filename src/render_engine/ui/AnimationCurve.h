#pragma once

#include <cmath>
#include <functional>
namespace ui {
namespace AnimationCurve {

typedef std::function<float(float)> AnimationCurve;

constexpr float linear(const float x) { return x; }
constexpr float cos(const float x) { return std::cos(x * 2.0f * 3.1415f); }
} // namespace AnimationCurve
}; // namespace ui

;
