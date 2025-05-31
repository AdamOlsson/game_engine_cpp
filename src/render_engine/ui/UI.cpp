#include "UI.h"
#include "render_engine/ui/ElementProperties.h"

using namespace ui;

constexpr glm::vec3 GREEN = glm::vec3(0.0, 0.8, 0.0);
constexpr glm::vec3 RED = glm::vec3(8.0, 0.0, 0.0);

UI::UI(Layout &&layout) {}

bool is_inside(const ViewportPoint &cursor_pos, const ElementProperties &element) {
    auto local_point = cursor_pos - (element.center - element.dimension / 2.0f);
    bool inside_x_axis = local_point.x > 0 && local_point.x < element.dimension.x;
    bool inside_y_axis = local_point.y > 0 && local_point.y < element.dimension.y;
    return inside_x_axis && inside_y_axis;
}

State UI::update_state_using_cursor(const ViewportPoint &cursor_pos) {
    ui::ElementProperties ui_element{};
    ui_element.center = glm::vec2(0.0, 100.0);
    ui_element.dimension = glm::vec2(600.0, 200.0);
    ui_element.border.color = GREEN;
    ui_element.border.thickness = 10.0;
    ui_element.border.radius = 30.0;

    if (is_inside(cursor_pos, ui_element)) {
        ui_element.border.color = RED;
    }

    return State{.properties = ui_element};
}

/*State UI::update_state_using_keypress() {}*/
