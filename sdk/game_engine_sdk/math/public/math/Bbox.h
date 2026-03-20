#pragma once

#include "math/Vector2.h"

namespace math {

class Bbox {
  private:
    math::Vector2 m_top_left;
    math::Vector2 m_bot_right;

  public:
    Bbox() = default;

    Bbox(const math::Vector2 &top_left, const math::Vector2 &bot_right)
        : m_top_left(top_left), m_bot_right(bot_right) {}

    Bbox(const Bbox &) = default;
    Bbox(Bbox &&) = default;
    Bbox &operator=(const Bbox &) = default;
    Bbox &operator=(Bbox &&) = default;

    math::Vector2 center() const {
        return m_top_left + (m_bot_right - m_top_left) / 2.0f;
    };

    void offset(const math::Vector2 &value) {
        m_top_left += value;
        m_bot_right += value;
    }

    math::Vector2 size() const { return m_bot_right - m_top_left; }
};

} // namespace math
