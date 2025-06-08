#pragma once

#include "Coordinates.h"
#include "render_engine/Window.h"
#include "render_engine/ui/Menu.h"
#include "render_engine/ui/State.h"
namespace ui {

class UI {
  private:
    ui::Menu m_menu;
    ui::State m_current_menu_state;
    ui::Menu *m_current_menu;

    std::vector<size_t> m_menu_idx_trace;

    bool is_inside(const ViewportPoint &cursor_pos, const ElementProperties &element);

    ui::State &update_state_using_cursor(const ViewportPoint &cursor_pos);
    ui::State &update_state_using_keypress();
    ui::State &update_state_using_click_event(const MouseEvent mouse_event,
                                              const ViewportPoint &cursor_pos);

  public:
    UI() = default;
    UI(Menu &menu);
    ~UI() = default;

    ui::State &get_state();
    ui::State &update_state_from_mouse_event(const MouseEvent mouse_event,
                                             const ViewportPoint &cursor_pos);
    // TODO: These can probably be merged to a single function and use event based args
};

}; // namespace ui
