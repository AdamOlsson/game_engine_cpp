#pragma once
#include "render_engine/colors.h"
#include "render_engine/ui/ElementProperties.h"

namespace ui {
class Layout {
  private:
  public:
    ui::ElementProperties m_element;

    constexpr Layout() {
        m_element.center = glm::vec2(0.0, 100.0);
        m_element.dimension = glm::vec2(600.0, 200.0);
        m_element.border.color = colors::GREEN;
        m_element.border.thickness = 10.0;
        m_element.border.radius = 30.0;
    }
    constexpr ~Layout() = default;
};

} // namespace ui
