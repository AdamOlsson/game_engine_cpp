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
  public:
    // TODO: This is wrong but only here for testing purposes
    ui::ElementProperties m_element;

    UI() = default;
    UI(ElementProperties &element);
    ~UI() = default;

    ui::State update_state_using_cursor(const ViewportPoint &cursor_pos);
    ui::State update_state_using_keypress();
};

}; // namespace ui
