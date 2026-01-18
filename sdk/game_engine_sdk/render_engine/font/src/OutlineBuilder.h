#pragma once

#include "math/interpolate.h"
#include "math/winding.h"
#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace font {

class OutlineBuilder {
  public:
    OutlineBuilder()
        : m_funcs({
              .move_to = &move_to_cb,
              .line_to = &line_to_cb,
              .conic_to = &conic_to_cb,
              .cubic_to = &cubic_to_cb,
              .shift = 0,
              .delta = 0,
          }) {}

    ~OutlineBuilder() {}

    FT_Outline_Funcs *funcs() { return &m_funcs; }

    std::pair<float, float> current_point;
    std::vector<std::vector<std::array<std::pair<float, float>, 3>>> quadratic_curves;
    std::vector<std::vector<std::pair<float, float>>> line_segments;

  private:
    FT_Outline_Funcs m_funcs;

    static const size_t m_num_segments = 8; // Number of segments to split curves into

    static int move_to_cb(const FT_Vector *to, void *user_data) {
        /*std::cout << std::format("M {} {}", to->x, to->y) << std::endl;*/
        OutlineBuilder *builder = static_cast<OutlineBuilder *>(user_data);

        builder->line_segments.emplace_back();
        builder->quadratic_curves.emplace_back();

        builder->line_segments.back().emplace_back(to->x, to->y);
        builder->current_point = {to->x, to->y};
        return 0;
    }

    static int line_to_cb(const FT_Vector *to, void *user_data) {
        /*std::cout << std::format("L {} {}", to->x, to->y) << std::endl;*/
        OutlineBuilder *builder = static_cast<OutlineBuilder *>(user_data);

        builder->line_segments.back().emplace_back(to->x, to->y);
        builder->current_point = {to->x, to->y};
        return 0;
    }

    static int conic_to_cb(const FT_Vector *control, const FT_Vector *to,
                           void *user_data) {
        /*std::cout << std::format("C {} {} {} {}", control->x, control->y, to->x,
         * to->y)*/
        /*          << std::endl;*/
        /*std::cout << "conic_to_cb" << std::endl;*/
        OutlineBuilder *builder = static_cast<OutlineBuilder *>(user_data);

        const std::pair<float, float> p0 = builder->current_point;
        const std::pair<float, float> p1 = {control->x, control->y};
        const std::pair<float, float> p2 = {to->x, to->y};

        builder->quadratic_curves.back().push_back({p0, p1, p2});

        const std::array<std::pair<float, float>, 3> triangle = {p0, p1, p2};

        // Note: I have assume that font files that use quadratic bezier curves have they
        // exterior in clockwise winding (although is reversed because I flip the y-axis
        // in FreeType). If I ever get a font that does use clockwise winding for the
        // exterior, I need to implement logic here to check for winding based on format
        // of the glyph.
        /*if (math::is_clockwise_winding(triangle)) {*/
        if (math::is_clockwise_winding(triangle)) {
            builder->line_segments.back().push_back(p1);
        } else {

            for (size_t i = 0; i < m_num_segments; i++) {
                const float t = i / static_cast<float>(m_num_segments);
                builder->line_segments.back().emplace_back(
                    math::QuadraticBezer::eval(p0, p1, p2, t));
            }
        }

        builder->line_segments.back().emplace_back(to->x, to->y);
        builder->current_point = {to->x, to->y};
        return 0;
    }

    static int cubic_to_cb(const FT_Vector *control1, const FT_Vector *control2,
                           const FT_Vector *to, void *user_data) {
        /*std::cout << std::format("C {} {} {} {} {} {}", to->x, to->y, control1->x,*/
        /*                         control1->y, control2->x, control2->y)*/
        /*          << std::endl;*/
        /*std::cout << "cubic_to_cb" << std::endl;*/
        OutlineBuilder *builder = static_cast<OutlineBuilder *>(user_data);

        const std::pair<float, float> p0 = builder->current_point;
        const std::pair<float, float> p1 = {control1->x, control1->y};
        const std::pair<float, float> p2 = {control2->x, control2->y};
        const std::pair<float, float> p3 = {to->x, to->y};

        const std::pair<math::CubicBezier, math::CubicBezier> split =
            math::de_casteljaus(p0, p1, p2, p3);
        const math::QuadraticBezer first_half =
            math::approximate_quadratic_bezier(split.first);
        const math::QuadraticBezer second_half =
            math::approximate_quadratic_bezier(split.second);

        builder->quadratic_curves.back().push_back(
            {first_half.p0, first_half.p1, first_half.p2});

        builder->quadratic_curves.back().push_back(
            {second_half.p0, second_half.p1, second_half.p2});

        // Note: this if statement assumes that curves always are defined in a counter
        // clockise order. Could be wrong though.
        const std::array<std::pair<float, float>, 3> triangle = {
            first_half.p0, first_half.p1, first_half.p2};

        // Note: I have assume that font files that use cubic bezier curves have they
        // exterior in counter clockwise winding (although is reversed because I flip the
        // y-axis in FreeType). If I ever get a font that does use counter clockwise
        // winding for the exterior, I need to implement logic here to check for winding
        // based on format of the glyph.
        /*if (math::signed_area_triangle(triangle) < 0.0f) {*/
        if (math::is_counter_clockwise_winding(triangle)) {
            builder->line_segments.back().push_back(first_half.p1);
            builder->line_segments.back().push_back(second_half.p1);
        } else {

            // Split the first half into segments
            for (size_t i = 0; i < m_num_segments; i++) {
                const float t = i / static_cast<float>(m_num_segments);
                builder->line_segments.back().emplace_back(first_half.eval(t));
            }

            // Split the second half into segments
            for (size_t i = 0; i < m_num_segments; i++) {
                const float t = i / static_cast<float>(m_num_segments);
                builder->line_segments.back().emplace_back(second_half.eval(t));
            }
        }

        builder->line_segments.back().emplace_back(to->x, to->y);
        builder->current_point = {to->x, to->y};
        return 0;
    }
};
} // namespace font
