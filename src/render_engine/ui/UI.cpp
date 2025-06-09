#include "UI.h"
#include "render_engine/ui/Button.h"
#include "render_engine/ui/ElementProperties.h"
#include <iostream>
#include <stdexcept>

using namespace ui;

// TODO:
// - Animations (start and stop animations based on the event callbacks)
//      - Idle animations
//      - event animations
// - Button text
// - Output text fields
// - Improve resolution of rounded corners
// - Improve resolution of text
// - Performance window

UI::UI(Menu &menu)
    : m_menu(std::move(menu)), m_current_menu_state(ui::State()),
      m_menu_trace({&m_menu}) {
    m_last_menu = get_last_menu();
    m_current_menu_state.properties = m_last_menu->properties_vector;
    m_menu.link(this);
}

[[nodiscard]] ui::State &UI::get_state() { return m_current_menu_state; }

void UI::push_new_menu(Menu *new_menu) {
    m_menu_trace.push_back(new_menu);
    m_last_menu = get_last_menu();
}

void UI::pop_menu() {
    m_menu_trace.pop_back();
    m_last_menu = get_last_menu();
}

Menu *UI::get_last_menu() {
    if (m_menu_trace.size() <= 0) {
        std::runtime_error("UI: Invalid call to get_last_menu()");
    }
    return m_menu_trace[m_menu_trace.size() - 1];
}

ui::State &UI::update_state_from_mouse_event(const MouseEvent mouse_event,
                                             const ViewportPoint &cursor_pos) {

    switch (mouse_event) {
    case MouseEvent::CURSOR_MOVED:
        return update_state_using_cursor(cursor_pos);
    case MouseEvent::LEFT_BUTTON_DOWN:
    case MouseEvent::LEFT_BUTTON_UP:
    case MouseEvent::RIGHT_BUTTON_DOWN:
    case MouseEvent::RIGHT_BUTTON_UP:
        return update_state_using_click_event(mouse_event, cursor_pos);
        break;
    }
    return m_current_menu_state;
}

ui::State &UI::update_state_using_click_event(const MouseEvent mouse_event,
                                              const ViewportPoint &cursor_pos) {
    // Find the item the cursor is over
    Button *target_button = nullptr;
    Menu *current_menu = m_last_menu;
    for (auto &button : current_menu->button_vector) {
        // TODO: We should also filter for z-axis here, eventually
        if (is_inside(cursor_pos, button->properties)) {
            target_button = button;
        }
    }

    if (target_button == nullptr) {
        return m_current_menu_state;
    }

    switch (mouse_event) {
    case MouseEvent::CURSOR_MOVED:
        break;
    case MouseEvent::LEFT_BUTTON_DOWN:
        target_button->click_state = ButtonClickState::Pressed;
        target_button->on_click(*target_button);
        break;
    case MouseEvent::LEFT_BUTTON_UP:
        target_button->click_state = ButtonClickState::Unpressed;
        target_button->on_unclick(*target_button);
        break;
    case MouseEvent::RIGHT_BUTTON_DOWN:
        break;
    case MouseEvent::RIGHT_BUTTON_UP:
        break;
    };

    m_current_menu_state.properties = m_last_menu->properties_vector;
    return m_current_menu_state;
}

State &UI::update_state_using_cursor(const ViewportPoint &cursor_pos) {
    Menu *current_menu = m_last_menu;
    for (auto &button : current_menu->button_vector) {
        bool cursor_is_inside = is_inside(cursor_pos, button->properties);
        if (cursor_is_inside) {
            switch (button->cursor_state) {
            case ui::ButtonCursorState::Leave:
            case ui::ButtonCursorState::Outside:
                button->cursor_state = ButtonCursorState::Enter;
                button->on_enter(*button);
                break;
            case ui::ButtonCursorState::Enter:
            case ui::ButtonCursorState::Hover:
                button->cursor_state = ButtonCursorState::Hover;
                button->on_hover(*button);
                break;
            default:
                break;
            };
        } else {
            switch (button->cursor_state) {
            case ui::ButtonCursorState::Leave:
            case ui::ButtonCursorState::Outside:
                button->cursor_state = ButtonCursorState::Outside;
                break;
            case ui::ButtonCursorState::Enter:
            case ui::ButtonCursorState::Hover:
                button->cursor_state = ButtonCursorState::Leave;
                button->on_leave(*button);
                break;
            default:
                break;
            };
        }
    }

    m_current_menu_state.properties = m_last_menu->properties_vector;
    return m_current_menu_state;
}

/*State UI::update_state_using_keypress() {}*/

bool UI::is_inside(const ViewportPoint &cursor_pos, const ElementProperties &element) {
    auto local_point = cursor_pos - (element.center - element.dimension / 2.0f);
    bool inside_x_axis = local_point.x > 0 && local_point.x < element.dimension.x;
    bool inside_y_axis = local_point.y > 0 && local_point.y < element.dimension.y;
    return inside_x_axis && inside_y_axis;
}
