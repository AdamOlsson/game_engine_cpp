#pragma once

#include "WindowDimension.h"

namespace window {

struct WindowConfig {
    WindowDimension<int> dims;
    char const *title;
};

} // namespace window
