#pragma once

#include "Coordinates.h"
#include "render_engine/ui/State.h"
namespace ui {

class MenuLayout {};

// TODO: How do we do animations?

// The following two items I donot want to decide until I know how animations work
// TODO: How do we transition between menus?
// TODO: Should we distinguish on-click between a menu transition and a leaf menu item?
class Layout {
  private:
  public:
    constexpr Layout() = default;
    constexpr ~Layout() = default;
};

class UI {
  private:
  public:
    UI(Layout &&layout);
    ~UI() = default;

    ui::State update_state_using_cursor(const ViewportPoint &cursor_pos);
    ui::State update_state_using_keypress();
};

}; // namespace ui
