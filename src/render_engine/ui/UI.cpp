#include "UI.h"
#include "render_engine/colors.h"
#include "render_engine/ui/ElementProperties.h"

using namespace ui;

UI::UI(ElementProperties &element) : m_element(std::move(element)) {}

bool is_inside(const ViewportPoint &cursor_pos, const ElementProperties &element) {
    auto local_point = cursor_pos - (element.center - element.dimension / 2.0f);
    bool inside_x_axis = local_point.x > 0 && local_point.x < element.dimension.x;
    bool inside_y_axis = local_point.y > 0 && local_point.y < element.dimension.y;
    return inside_x_axis && inside_y_axis;
}

State UI::update_state_using_cursor(const ViewportPoint &cursor_pos) {
    m_element.border.color = colors::GREEN;

    if (is_inside(cursor_pos, m_element)) {
        m_element.border.color = colors::RED;
    }

    return State{.properties = m_element};
}

/*State UI::update_state_using_keypress() {}*/
