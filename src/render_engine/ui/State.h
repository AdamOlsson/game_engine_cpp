#pragma once

#include "render_engine/ui/ElementProperties.h"
#include <vector>
namespace ui {
// Contains the properties of each element in the layout
struct State {
    std::vector<ElementProperties *> properties;
};

}; // namespace ui
