#pragma once

#include "Coordinates.h"
#include "render_engine/Window.h"
#include "render_engine/ui/Menu.h"
#include "render_engine/ui/State.h"
#include "render_engine/ui/TextBox.h"
#include <unordered_map>
namespace ui {

class UI {
  private:
    std::unordered_map<std::string, TextBox> m_text_boxes;
    std::vector<TextBox *> m_text_boxes_state;

    ui::Menu m_menu;
    ui::State m_current_menu_state;
    std::vector<ui::Menu *> m_menu_trace;
    Menu *m_last_menu_in_trace;

    bool is_inside(const ViewportPoint &cursor_pos, const ElementProperties &element);

    Menu *get_last_menu();

    // TODO: These can probably be merged to a single function and use event based args
    ui::State &update_state_using_cursor(const ViewportPoint &cursor_pos);
    ui::State &update_state_using_keypress();
    ui::State &update_state_using_click_event(const MouseEvent mouse_event,
                                              const ViewportPoint &cursor_pos);

  public:
    UI() = default;
    UI(Menu &menu);
    ~UI() = default;

    void set_menu(Menu &&menu);
    void add_text_box(const std::string &&id, TextBox &&text_box);
    void add_text_box(const std::string &id, TextBox &&text_box);
    TextBox &get_text_box(std::string &id);

    ui::State &get_state();
    ui::State &update_state_from_mouse_event(const MouseEvent mouse_event,
                                             const ViewportPoint &cursor_pos);

    void push_new_menu(Menu *new_menu);
    void pop_menu();
};

}; // namespace ui
