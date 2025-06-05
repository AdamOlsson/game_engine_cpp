#include "UI.h"
#include "render_engine/colors.h"
#include "render_engine/ui/ElementProperties.h"

using namespace ui;

class Button {
  public:
    std::function<void(void)> on_click;
    Button() {}
};

class Layout {
  public:
    std::vector<std::vector<Button>> items;
    Layout(std::vector<std::vector<Button>> items) : items(items) {}
};

UI::UI(std::vector<std::unique_ptr<ui::ElementProperties>> elements)
    : m_elements(std::move(elements)) {
    // This should be created by the user (but in a nicer way, look at obisidian)
    std::vector<Button> submenu = {Button(), Button()};
    std::vector<Button> single_item = {Button()};
    std::vector<std::vector<Button>> items = {submenu, single_item};
    Layout layout = Layout(items);

    for (auto item : layout.items) {
        if (item.size() > 1) {
            // submenu
        } else {
            // button
        }
    }
}

ui::State UI::click_event(const ViewportPoint &cursor_pos) {

    std::vector<std::vector<Button>> items = {};
    std::vector<Button> clicked_item = items[0];
    if (clicked_item.size() > 1) {
        // submenu
        // CONTINUE: Update the state such that it is filled with the submenus Buttons
        // properties
        return State{.properties = {}}; // Wrong, temporary for now
    } else {
        // button
        clicked_item[0].on_click();
        return State{.properties = {}};
    }
}

bool is_inside(const ViewportPoint &cursor_pos, const ElementProperties &element) {
    auto local_point = cursor_pos - (element.center - element.dimension / 2.0f);
    bool inside_x_axis = local_point.x > 0 && local_point.x < element.dimension.x;
    bool inside_y_axis = local_point.y > 0 && local_point.y < element.dimension.y;
    return inside_x_axis && inside_y_axis;
}

State UI::update_state_using_cursor(const ViewportPoint &cursor_pos) {
    std::vector<ElementProperties *> properties;
    for (auto &element : m_elements) {
        element->border.color = colors::GREEN;
        if (is_inside(cursor_pos, *element)) {
            element->border.color = colors::RED;
        }
        properties.push_back(element.get());
    }
    return State{.properties = properties};
}

/*State UI::update_state_using_keypress() {}*/
