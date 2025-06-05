#pragma once

#include "Coordinates.h"
#include "render_engine/ui/ElementProperties.h"
#include "render_engine/ui/State.h"
namespace ui {

// The following two items I donot want to decide until I know how animations work
// TODO: How do we transition between menus?
// TODO: Should we distinguish on-click between a menu transition and a leaf menu item?

class UI {
  private:
    std::vector<std::unique_ptr<ui::ElementProperties>> m_elements;

  public:
    UI() = default;
    UI(std::vector<std::unique_ptr<ui::ElementProperties>> elements);
    ~UI() = default;

    // TODO: These can probably be merged to a single function and use event based args
    ui::State update_state_using_cursor(const ViewportPoint &cursor_pos);
    ui::State update_state_using_keypress();
    ui::State click_event(const ViewportPoint &cursor_pos);
};

}; // namespace ui
