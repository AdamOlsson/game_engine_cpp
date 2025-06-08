#include "UI.h"
#include "render_engine/ui/Button.h"
#include "render_engine/ui/ElementProperties.h"

using namespace ui;

UI::UI(Menu &menu)
    : m_menu(menu), m_current_menu_state(ui::State()), m_menu_idx_trace({}),
      m_current_menu(&m_menu) {
    m_current_menu_state.properties = m_current_menu->properties_iterator();
}

[[nodiscard]] ui::State &UI::get_state() { return m_current_menu_state; }

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
    for (auto &button : m_current_menu->button_iterator()) {
        // TODO: We should also filter for z-axis here, eventually
        if (is_inside(cursor_pos, button->properties)) {
            target_button = button;
        }
    }

    if (target_button == nullptr) {
        return m_current_menu_state;
    }

    // CONTINUE: During click we need to distinguish between a submenu button and a
    // regular button as the submenu button should also update which items are visible.
    // Once i've done that I need to figure out how to go back up the menu tree
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

    /*for (auto i = 0; i < m_current_menu->num_items; i++) {*/
    /*    if (m_current_menu->submenus[i].has_value()) {*/
    /*        bool is_click_inside_submenu = false;*/
    /*        // TODO: The main questions now is how should I keep track of what submenu*/
    /*        // should be displayed on screen and also keeping track of the full layout*/
    /*        if (is_click_inside_submenu) {*/
    /*            m_menu_idx_trace.push_back(i);*/
    /*            m_current_menu = &m_current_menu->submenus[i].value();*/
    /*            // TODO: Return a state create from the submenu*/
    /*        }*/
    /*    } else if (m_current_menu->buttons[i].has_value()) {*/
    /*        Button &button = m_current_menu->buttons[i].value();*/
    /*        bool is_click_inside_button = false;*/
    /*        if (is_inside(cursor_pos, button.properties)) {*/
    /*            button.on_click(button);*/
    /*        }*/
    /*    }*/
    /*}*/

    m_current_menu_state.properties = m_current_menu->properties_iterator();
    return m_current_menu_state;
}

State &UI::update_state_using_cursor(const ViewportPoint &cursor_pos) {
    for (auto &button : m_current_menu->button_iterator()) {
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

    m_current_menu_state.properties = m_current_menu->properties_iterator();
    return m_current_menu_state;
}

/*State UI::update_state_using_keypress() {}*/

bool UI::is_inside(const ViewportPoint &cursor_pos, const ElementProperties &element) {
    auto local_point = cursor_pos - (element.center - element.dimension / 2.0f);
    bool inside_x_axis = local_point.x > 0 && local_point.x < element.dimension.x;
    bool inside_y_axis = local_point.y > 0 && local_point.y < element.dimension.y;
    return inside_x_axis && inside_y_axis;
}
