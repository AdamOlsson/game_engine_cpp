#pragma once

#include <ft2build.h>
#include <iostream>
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

    std::vector<std::vector<std::pair<float, float>>> line_segments;
    // std::vector<std::pair<float, float>> curve_segments;

  private:
    FT_Outline_Funcs m_funcs;

    static int move_to_cb(const FT_Vector *to, void *user_data) {
        std::cout << std::format("M {} {}", to->x, to->y) << std::endl;
        OutlineBuilder *builder = static_cast<OutlineBuilder *>(user_data);

        builder->line_segments.emplace_back();

        builder->line_segments.back().emplace_back(to->x, to->y);
        return 0;
    }

    static int line_to_cb(const FT_Vector *to, void *user_data) {
        std::cout << std::format("L {} {}", to->x, to->y) << std::endl;
        OutlineBuilder *builder = static_cast<OutlineBuilder *>(user_data);

        builder->line_segments.back().emplace_back(to->x, to->y);
        return 0;
    }

    static int conic_to_cb(const FT_Vector *control, const FT_Vector *to,
                           void *user_data) {
        std::cout << std::format("C {} {} {} {}", control->x, control->y, to->x, to->y)
                  << std::endl;
        OutlineBuilder *builder = static_cast<OutlineBuilder *>(user_data);

        // TODO: Not correct
        builder->line_segments.back().emplace_back(to->x, to->y);
        return 0;
    }

    static int cubic_to_cb(const FT_Vector *control1, const FT_Vector *control2,
                           const FT_Vector *to, void *user_data) {
        std::cout << std::format("C {} {} {} {} {} {}", to->x, to->y, control1->x,
                                 control1->y, control2->x, control2->y)
                  << std::endl;
        OutlineBuilder *builder = static_cast<OutlineBuilder *>(user_data);

        // TODO: Not correct
        builder->line_segments.back().emplace_back(to->x, to->y);
        return 0;
    }
};
} // namespace font
