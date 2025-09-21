#include "game_engine_sdk/render_engine/ui/UI.h"
#include "game_engine_sdk/render_engine/ui/Button.h"
#include "game_engine_sdk/render_engine/ui/ElementProperties.h"
#include <stdexcept>

using namespace ui;

UI::UI(Menu &menu)
    : m_menu(std::move(menu)), m_current_menu_state(ui::State()),
      m_menu_trace({&m_menu}) {
    m_last_menu_in_trace = get_last_menu();
    m_menu.link(this);
    m_current_menu_state.buttons = m_last_menu_in_trace->button_vector;
}

UI &UI::operator=(UI &&other) noexcept {
    if (this != &other) {
        m_text_boxes = std::move(m_text_boxes);
        m_text_boxes_state.reserve(other.m_text_boxes_state.size());
        for (auto &pair : m_text_boxes) {
            m_text_boxes_state.push_back(&pair.second);
        }

        m_menu = std::move(other.m_menu);
        m_current_menu_state = ui::State();
        m_menu_trace = {&m_menu};
        m_last_menu_in_trace = get_last_menu();
        m_menu.link(this);
        m_current_menu_state.buttons = m_last_menu_in_trace->button_vector;

        other.m_text_boxes_state.clear();
        other.m_current_menu_state = ui::State();
        other.m_menu_trace.clear();
        other.m_last_menu_in_trace = nullptr;
    }
    return *this;
}

void UI::add_text_box(const std::string &id, TextBox &&text_box) {
    m_text_boxes.insert({id, std::move(text_box)});
    m_text_boxes_state.push_back(&m_text_boxes[id]);
}

void UI::add_text_box(const std::string &&id, TextBox &&text_box) {
    m_text_boxes.insert({id, std::move(text_box)});
    m_text_boxes_state.push_back(&m_text_boxes[id]);
}

TextBox &UI::get_text_box(const std::string &id) { return m_text_boxes[id]; }

[[nodiscard]] ui::State &UI::get_state() {
    for (auto button : m_last_menu_in_trace->button_vector) {
        button->animations.step_animations();
    }

    m_current_menu_state.text_boxes = m_text_boxes_state;
    return m_current_menu_state;
}

void UI::push_new_menu(Menu *new_menu) {
    m_menu_trace.push_back(new_menu);
    m_last_menu_in_trace = get_last_menu();
}

void UI::pop_menu() {
    m_menu_trace.pop_back();
    m_last_menu_in_trace = get_last_menu();
}

Menu *UI::get_last_menu() {
    if (m_menu_trace.size() <= 0) {
        std::runtime_error("UI: Invalid call to get_last_menu()");
    }
    return m_menu_trace[m_menu_trace.size() - 1];
}

ui::State &UI::update_state_from_mouse_event(const window::MouseEvent mouse_event,
                                             const window::ViewportPoint &cursor_pos) {

    switch (mouse_event) {
    case window::MouseEvent::CURSOR_MOVED:
        return update_state_using_cursor(cursor_pos);
    case window::MouseEvent::LEFT_BUTTON_DOWN:
    case window::MouseEvent::LEFT_BUTTON_UP:
    case window::MouseEvent::RIGHT_BUTTON_DOWN:
    case window::MouseEvent::RIGHT_BUTTON_UP:
        return update_state_using_click_event(mouse_event, cursor_pos);
        break;
    }
    return m_current_menu_state;
}

ui::State &UI::update_state_using_click_event(const window::MouseEvent mouse_event,
                                              const window::ViewportPoint &cursor_pos) {
    // Find the item the cursor is over
    Button *target_button = nullptr;
    Menu *current_menu = m_last_menu_in_trace;
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
    case window::MouseEvent::CURSOR_MOVED:
        break;
    case window::MouseEvent::LEFT_BUTTON_DOWN:
        target_button->click_state = ButtonClickState::Pressed;
        target_button->on_click(*target_button);
        break;
    case window::MouseEvent::LEFT_BUTTON_UP:
        target_button->click_state = ButtonClickState::Unpressed;
        target_button->on_unclick(*target_button);
        break;
    case window::MouseEvent::RIGHT_BUTTON_DOWN:
        break;
    case window::MouseEvent::RIGHT_BUTTON_UP:
        break;
    };

    m_current_menu_state.buttons = m_last_menu_in_trace->button_vector;
    return m_current_menu_state;
}

State &UI::update_state_using_cursor(const window::ViewportPoint &cursor_pos) {
    Menu *current_menu = m_last_menu_in_trace;
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

    m_current_menu_state.buttons = m_last_menu_in_trace->button_vector;
    return m_current_menu_state;
}

/*State UI::update_state_using_keypress() {}*/

bool UI::is_inside(const window::ViewportPoint &cursor_pos,
                   const ElementProperties &element) {
    float half_width = element.container.dimension.x / 2.0f;
    float half_height = element.container.dimension.y / 2.0f;

    return (cursor_pos.x >= element.container.center.x - half_width) &&
           (cursor_pos.x <= element.container.center.x + half_width) &&
           (cursor_pos.y >= element.container.center.y - half_height) &&
           (cursor_pos.y <= element.container.center.y + half_height);
}
