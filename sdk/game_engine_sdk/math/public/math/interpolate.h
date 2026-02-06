#pragma once

#include "math/traits.h"
#include <algorithm>
#include <format>
#include <utility>
#include <vector>
namespace math {

struct QuadraticBezer {
    const std::pair<float, float> p0 = {0, 0};
    const std::pair<float, float> p1 = {0, 0};
    const std::pair<float, float> p2 = {0, 0};

    static std::pair<float, float> eval(const std::pair<float, float> &p0,
                                        const std::pair<float, float> &p1,
                                        const std::pair<float, float> &p2,
                                        const float t) {
        const float t2 = t * t;
        const float mt = 1.0f - t;
        const float mt2 = mt * mt;
        return {
            mt2 * p0.first + 2.0f * mt * t * p1.first + t2 * p2.first,
            mt2 * p0.second + 2.0f * mt * t * p1.second + t2 * p2.second,
        };
    }

    std::pair<float, float> eval(const float t) const {
        const float t2 = t * t;
        const float mt = 1.0f - t;
        const float mt2 = mt * mt;
        return {
            mt2 * p0.first + 2.0f * mt * t * p1.first + t2 * p2.first,
            mt2 * p0.second + 2.0f * mt * t * p1.second + t2 * p2.second,
        };
    }

    static float calculate_quadratic_flatness(const QuadraticBezer &curve) {
        // Distance from control point to the line p0-p2
        float dx = curve.p2.first - curve.p0.first;
        float dy = curve.p2.second - curve.p0.second;
        float len_sq = dx * dx + dy * dy;

        if (len_sq < 1e-6f)
            return 0.0f;

        float t = ((curve.p1.first - curve.p0.first) * dx +
                   (curve.p1.second - curve.p0.second) * dy) /
                  len_sq;
        t = std::max(0.0f, std::min(1.0f, t));

        float proj_x = curve.p0.first + t * dx;
        float proj_y = curve.p0.second + t * dy;
        float dist_x = curve.p1.first - proj_x;
        float dist_y = curve.p1.second - proj_y;

        return std::sqrt(dist_x * dist_x + dist_y * dist_y);
    }
};

struct CubicBezier {
    const std::pair<float, float> p0 = {0, 0};
    const std::pair<float, float> p1 = {0, 0};
    const std::pair<float, float> p2 = {0, 0};
    const std::pair<float, float> p3 = {0, 0};

    static std::pair<float, float> eval(const std::pair<float, float> &p0,
                                        const std::pair<float, float> &p1,
                                        const std::pair<float, float> &p2,
                                        const std::pair<float, float> &p3,
                                        const float t) {
        const float t2 = t * t;
        const float t3 = t2 * t;
        const float mt = 1.0f - t;
        const float mt2 = mt * mt;
        const float mt3 = mt2 * mt;

        return {
            mt3 * p0.first + 3.0f * mt2 * t * p1.first + 3.0f * mt * t2 * p2.first +
                t3 * p3.first,
            mt3 * p0.second + 3.0f * mt2 * t * p1.second + 3.0f * mt * t2 * p2.second +
                t3 * p3.second,
        };
    }

    std::pair<float, float> eval(const float t) const {
        const float t2 = t * t;
        const float t3 = t2 * t;
        const float mt = 1.0f - t;
        const float mt2 = mt * mt;
        const float mt3 = mt2 * mt;

        return {
            mt3 * p0.first + 3.0f * mt2 * t * p1.first + 3.0f * mt * t2 * p2.first +
                t3 * p3.first,
            mt3 * p0.second + 3.0f * mt2 * t * p1.second + 3.0f * mt * t2 * p2.second +
                t3 * p3.second,
        };
    };
};

template <Point T> constexpr T lerp(const T &A, const T &B, const float t) {
    if constexpr (std::same_as<T, std::pair<float, float>>) {
        return std::make_pair((1.0f - t) * A.first + t * B.first,
                              (1.0f - t) * A.second + t * B.second);
    } else if constexpr (std::same_as<T, glm::vec2>) {
        return glm::vec2((1.0f - t) * A.x + t * B.x, (1.0f - t) * A.y + t * B.y);
    } else if constexpr (std::same_as<T, glm::vec3>) {
        return glm::vec3((1.0f - t) * A.x + t * B.x, (1.0f - t) * A.y + t * B.y,
                         (1.0f - t) * A.z + t * B.z);
    } else {
        throw std::runtime_error(std::format("Error: lerp() not implemented for type."));
    }
}

constexpr std::pair<math::CubicBezier, math::CubicBezier>
de_casteljaus(const std::pair<float, float> &p0, const std::pair<float, float> &p1,
              const std::pair<float, float> &p2, const std::pair<float, float> &p3) {
    const std::pair<float, float> A = lerp(p0, p1, 0.5f);
    const std::pair<float, float> B = lerp(p1, p2, 0.5f);
    const std::pair<float, float> C = lerp(p2, p3, 0.5f);

    const std::pair<float, float> D = lerp(A, B, 0.5f);
    const std::pair<float, float> E = lerp(B, C, 0.5f);

    const std::pair<float, float> F = lerp(D, E, 0.5f);

    return std::make_pair(math::CubicBezier{p0, A, D, F}, math::CubicBezier{F, E, C, p3});
}

constexpr std::pair<math::CubicBezier, math::CubicBezier>
de_casteljaus(const CubicBezier &bezier) {
    const std::pair<float, float> A = lerp(bezier.p0, bezier.p1, 0.5f);
    const std::pair<float, float> B = lerp(bezier.p1, bezier.p2, 0.5f);
    const std::pair<float, float> C = lerp(bezier.p2, bezier.p3, 0.5f);

    const std::pair<float, float> D = lerp(A, B, 0.5f);
    const std::pair<float, float> E = lerp(B, C, 0.5f);

    const std::pair<float, float> F = lerp(D, E, 0.5f);

    return std::make_pair(math::CubicBezier{bezier.p0, A, D, F},
                          math::CubicBezier{F, E, C, bezier.p3});
}

constexpr math::QuadraticBezer
approximate_quadratic_bezier(const math::CubicBezier &cubic) {
    return math::QuadraticBezer{
        cubic.p0,
        std::make_pair(
            (3.0f * (cubic.p1.first + cubic.p2.first) - cubic.p0.first - cubic.p3.first) /
                4.0f,
            (3.0f * (cubic.p1.second + cubic.p2.second) - cubic.p0.second -
             cubic.p3.second) /
                4.0f),
        cubic.p3};
}

static void subdivide_quadratic_adaptive(const QuadraticBezer &curve,
                                         std::vector<QuadraticBezer> &output,
                                         float flatness_threshold, int max_depth = 8,
                                         int current_depth = 0) {
    float flatness = QuadraticBezer::calculate_quadratic_flatness(curve);

    // Base cases: curve is flat enough or max depth reached
    if (flatness <= flatness_threshold || current_depth >= max_depth) {
        output.push_back(curve);
        return;
    }

    // Subdivide at t=0.5 using De Casteljau's algorithm
    auto p01 = std::make_pair((curve.p0.first + curve.p1.first) * 0.5f,
                              (curve.p0.second + curve.p1.second) * 0.5f);
    auto p12 = std::make_pair((curve.p1.first + curve.p2.first) * 0.5f,
                              (curve.p1.second + curve.p2.second) * 0.5f);
    auto p012 =
        std::make_pair((p01.first + p12.first) * 0.5f, (p01.second + p12.second) * 0.5f);

    QuadraticBezer first_half{curve.p0, p01, p012};
    QuadraticBezer second_half{p012, p12, curve.p2};

    // Recursively subdivide both halves
    subdivide_quadratic_adaptive(first_half, output, flatness_threshold, max_depth,
                                 current_depth + 1);
    subdivide_quadratic_adaptive(second_half, output, flatness_threshold, max_depth,
                                 current_depth + 1);
}

static std::vector<QuadraticBezer> decompose_cubic_adaptive(
    const std::pair<float, float> &p0, const std::pair<float, float> &p1,
    const std::pair<float, float> &p2, const std::pair<float, float> &p3,
    float flatness_threshold = 0.5f) {

    std::vector<QuadraticBezer> result;

    // Split the cubic into two halves
    const std::pair<math::CubicBezier, math::CubicBezier> split =
        math::de_casteljaus(p0, p1, p2, p3);

    // Approximate each half as quadratic
    const QuadraticBezer first_half = math::approximate_quadratic_bezier(split.first);
    const QuadraticBezer second_half = math::approximate_quadratic_bezier(split.second);

    // Adaptively subdivide each quadratic approximation
    subdivide_quadratic_adaptive(first_half, result, flatness_threshold);
    subdivide_quadratic_adaptive(second_half, result, flatness_threshold);

    return result;
}

} // namespace math
