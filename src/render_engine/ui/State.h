#pragma once

#include "render_engine/ui/Button.h"
#include <vector>
namespace ui {
// Contains the properties of each element in the layout
struct State {
    // std::vector<TextBox *> text_boxes;
    std::vector<Button *> buttons;
};

}; // namespace ui
