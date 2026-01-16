#pragma once

#include <utility>
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

template <typename T>
constexpr std::pair<T, T> lerp(const std::pair<T, T> &A, const std::pair<T, T> &B,
                               const float t) {
    return std::make_pair((1.0f - t) * A.first + t * B.first,
                          (1.0f - t) * A.second + t * B.second);
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

} // namespace math
